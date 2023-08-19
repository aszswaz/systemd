#pragma once
#include <Windows.h>

#define SVCNAME TEXT("Systemd")

namespace Systemd {
	class Systemd {
		private:
			static STARTUPINFO si;
			static PROCESS_INFORMATION pi;
			static HANDLE jobHandle;
		public:
			static void start();
			static void stop();
	};
}
