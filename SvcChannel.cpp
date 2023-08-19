#include "SvcChannel.h"
#include "SystemdException.h"
#include "Systemd.h"
#include "Logger.h"

namespace Systemd {
	SERVICE_STATUS SvcChannel::status {};
	SERVICE_STATUS_HANDLE SvcChannel::statusHandle {};

	void SvcChannel::svcMain(DWORD dwArgc, LPTSTR* lpszArgv) {
		// 注册处理 SCM 指令的函数
		statusHandle = RegisterServiceCtrlHandler(SVCNAME, ctrlHandler);
		if (!statusHandle) {
			throw SystemdException("RegisterServiceCtrlHandler faild");
			return;
		}

		// 设置服务状态
		status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		status.dwServiceSpecificExitCode = 0;
		status.dwCurrentState = SERVICE_START_PENDING;
		status.dwControlsAccepted = 0;
		status.dwWin32ExitCode = 0;
		status.dwWaitHint = 3000;
		status.dwCheckPoint = 2;
		SetServiceStatus(statusHandle, &status);

		// 状态切换为运行中，可接受 STOP 指令
		status.dwCurrentState = SERVICE_RUNNING;
		status.dwCheckPoint = 0;
		status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		SetServiceStatus(statusHandle, &status);

		try {
			Systemd::start();
		} catch (SystemdException& e) {
			Logger::error(e.what());
		}

		// 向 SCM 报告服务已停止，SCM 随后会杀死进程
		status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(statusHandle, &status);
	}

	void WINAPI SvcChannel::ctrlHandler(DWORD dwCtrl) {
		switch (dwCtrl) {
		case SERVICE_CONTROL_STOP:
			status.dwCurrentState = SERVICE_STOP_PENDING;
			status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
			SetServiceStatus(statusHandle, &status);

			// TODO: 停止服务
			Systemd::stop();

			// 向 SCM 报告服务已停止，SCM 随后会杀死进程
			status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(statusHandle, &status);
			break;
		default:
			break;
		}
	}

	void SvcChannel::start() {
		int code = 0;

		// 连接到 SCM，main 线程被用于监听 SCM 的控制指令，链接成功后调用 SvcMain 函数
		SERVICE_TABLE_ENTRY tables[] = {
			{SVCNAME, (LPSERVICE_MAIN_FUNCTION)svcMain},
			{NULL, NULL}
		};
		code = StartServiceCtrlDispatcher(tables);
		if (!code) {
			code = GetLastError();
			switch (code) {
				case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
					throw SystemdException("Please start this program in Windows SCM.");
				default:
					throw SystemdException("Failed to start svc controller, code: " + std::to_string(GetLastError()));
			}
		}
	}
}