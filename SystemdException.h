#pragma once

#include <exception>
#include <string>

namespace Systemd {
	class SystemdException : public std::exception {
		private:
			std::string msg;

		public:
			SystemdException(std::string msg);
			const char *what();
	};
}
