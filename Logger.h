#pragma once
#include <string>

#include <windows.h>

using namespace std;

namespace Systemd {
	class Logger {
		private:
			static HANDLE logEvent;
		private:
			static void sendLog(WORD level, string msg);
		public:
			static void init();
			static void info(string msg);
			static void warn(string msg);
			static void error(string msg);
	};
}