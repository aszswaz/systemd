#include "Systemd.h"
#include "Logger.h"
#include "SystemdException.h"

namespace Systemd {
    STARTUPINFO Systemd::si {};
    PROCESS_INFORMATION Systemd::pi {};
    HANDLE Systemd::jobHandle {};

	void Systemd::start() {
        int code;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        jobHandle = CreateJobObject(nullptr, nullptr);
        if (!jobHandle) throw SystemdException("CreateJobObject faild");

        // 设置 Job 对象回收，自动退出所有子进程
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit_info;
        ZeroMemory(&limit_info, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
        limit_info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(jobHandle, JobObjectExtendedLimitInformation, &limit_info, sizeof(limit_info));

        // Start the child process. 
        code = CreateProcess(
            "C:\\Users\\aszswaz\\AppData\\Local\\V2ray\\bin\\v2ray.exe",           // No module name (use command line)
            "C:\\Users\\aszswaz\\AppData\\Local\\V2ray\\bin\\v2ray.exe run -confdir C:/Users/aszswaz/AppData/Local/V2ray",              // Command line
            NULL,                                                         // Process handle not inheritable
            NULL,                                                         // Thread handle not inheritable
            FALSE,                                                        // Set handle inheritance to FALSE
            0,                                                            // No creation flags
            NULL,                                                         // Use parent's environment block
            NULL,                                                         // Use parent's starting directory 
            &si,                                                          // Pointer to STARTUPINFO structure
            &pi);                                                         // Pointer to PROCESS_INFORMATION structure
        if (!code) throw SystemdException("CreateProcess failed.");

        // 关联 JOB 和进程
        AssignProcessToJobObject(jobHandle, pi.hProcess);
        // 等待子进程退出
        WaitForSingleObject(pi.hProcess, INFINITE);
        stop();
	}

    void Systemd::stop() {
        // Close process and thread handles. 
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(jobHandle);
    }
}