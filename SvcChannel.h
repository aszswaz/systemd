#pragma once

#include <Windows.h>

#include "systemd.h"

namespace Systemd {
	class SvcChannel {
		private:
			static SERVICE_STATUS status;
			static SERVICE_STATUS_HANDLE statusHandle;
			static HANDLE stopEvent;

		private:
			static void svcMain(DWORD dwArgc, LPTSTR* lpszArgv);
			static void ctrlHandler(DWORD dwCtrl);

		public:
			static void start();
	};
}