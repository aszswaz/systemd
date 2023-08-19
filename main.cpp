#include <iostream>

#include <windows.h>

#include "Systemd.h"
#include "Logger.h"
#include "SvcChannel.h"
#include "SystemdException.h"
#include "Systemd.h"

int main() {
	try {
		Systemd::Logger::init();
		Systemd::SvcChannel::start();
		// Systemd::Systemd::start();
		// Systemd::Systemd::stop();
	} catch (Systemd::SystemdException &e) {
		cout << e.what() << endl;
		Systemd::Logger::error(e.what());
		return 1;
	}
	return 0;
}