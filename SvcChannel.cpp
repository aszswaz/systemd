#include "SvcChannel.h"
#include "SystemdException.h"
#include "Systemd.h"
#include "Logger.h"

namespace Systemd {
	SERVICE_STATUS SvcChannel::status {};
	SERVICE_STATUS_HANDLE SvcChannel::statusHandle {};

	void SvcChannel::svcMain(DWORD dwArgc, LPTSTR* lpszArgv) {
		// ע�ᴦ�� SCM ָ��ĺ���
		statusHandle = RegisterServiceCtrlHandler(SVCNAME, ctrlHandler);
		if (!statusHandle) {
			throw SystemdException("RegisterServiceCtrlHandler faild");
			return;
		}

		// ���÷���״̬
		status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		status.dwServiceSpecificExitCode = 0;
		status.dwCurrentState = SERVICE_START_PENDING;
		status.dwControlsAccepted = 0;
		status.dwWin32ExitCode = 0;
		status.dwWaitHint = 3000;
		status.dwCheckPoint = 2;
		SetServiceStatus(statusHandle, &status);

		// ״̬�л�Ϊ�����У��ɽ��� STOP ָ��
		status.dwCurrentState = SERVICE_RUNNING;
		status.dwCheckPoint = 0;
		status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		SetServiceStatus(statusHandle, &status);

		try {
			Systemd::start();
		} catch (SystemdException& e) {
			Logger::error(e.what());
		}

		// �� SCM ���������ֹͣ��SCM ����ɱ������
		status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(statusHandle, &status);
	}

	void WINAPI SvcChannel::ctrlHandler(DWORD dwCtrl) {
		switch (dwCtrl) {
		case SERVICE_CONTROL_STOP:
			status.dwCurrentState = SERVICE_STOP_PENDING;
			status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
			SetServiceStatus(statusHandle, &status);

			// TODO: ֹͣ����
			Systemd::stop();

			// �� SCM ���������ֹͣ��SCM ����ɱ������
			status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(statusHandle, &status);
			break;
		default:
			break;
		}
	}

	void SvcChannel::start() {
		int code = 0;

		// ���ӵ� SCM��main �̱߳����ڼ��� SCM �Ŀ���ָ����ӳɹ������ SvcMain ����
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