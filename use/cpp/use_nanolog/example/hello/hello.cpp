#include "NanoLogCpp17.h"
#include <array>
using namespace NanoLog::LogLevels;

void run()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	NANO_LOG(NOTICE, "current time %llu.%lu", (unsigned long long)ts.tv_sec,
			 (unsigned long)ts.tv_nsec);
}

int main()
{
	NanoLog::setLogFile("test_log");
	NanoLog::preallocate();
	NanoLog::setLogLevel(NOTICE);

	for (int i = 0; i < 64; i++) {
		run();
	}

	return 0;
}
