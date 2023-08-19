#include "SystemdException.h"

namespace Systemd {
	SystemdException::SystemdException(std::string msg) {
		this->msg = msg;
	}
	const char* SystemdException::what() {
		return this->msg.c_str();
	}
}