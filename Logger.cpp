#include <iostream>

#include <strsafe.h>

#include "Logger.h"
#include "Systemd.h"

#define SVC_ERROR ((DWORD)1000)

using namespace std;

namespace Systemd {
    HANDLE Logger::logEvent {};

    void Logger::init() {
        logEvent = RegisterEventSource(NULL, SVCNAME);
    }

    void Logger::sendLog(WORD level, string msg) {
        LPCTSTR lpszStrings[2]; 
        int code = 0;

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = msg.c_str();

        code = ReportEvent(
            logEvent,            // event log handle
            level,               // event type
            0,                   // event category
            SVC_ERROR,           // event identifier
            NULL,                // no security identifier
            2,                   // size of lpszStrings array
            0,                   // no binary data
            lpszStrings,         // array of strings
            NULL                 // no binary data
        );
        if (!code) {
            code = GetLastError();
            switch (code) {
                case ERROR_INVALID_PARAMETER:
                    cerr << "ERROR_INVALID_PARAMETER" << endl;
                    break;
                case ERROR_NOT_ENOUGH_MEMORY:
                    cerr << "ERROR_NOT_ENOUGH_MEMORY" << endl;
                    break;
                case RPC_S_INVALID_BOUND:
                    cerr << "RPC_S_INVALID_BOUND" << endl;
                    break;
                case RPC_X_BAD_STUB_DATA:
                    cerr << "RPC_X_BAD_STUB_DATA" << endl;
                    break;
            }
        }

        DeregisterEventSource(logEvent);
    }

    void Logger::info(string msg) {
        sendLog(EVENTLOG_INFORMATION_TYPE, msg);
    }

    void Logger::warn(string msg) {
        sendLog(EVENTLOG_WARNING_TYPE, msg);
    }

    void Logger::error(string msg) {
        sendLog(EVENTLOG_ERROR_TYPE, msg);
    }
}