#include "muggle/c/muggle_c.h"
#include <time.h>

void clear_screen()
{
	printf("\033[2J\033[1;1H");
}

void show_datetime()
{
	struct timespec ts;
	struct tm t;
	char datetime[128];

	timespec_get(&ts, TIME_UTC);
	localtime_r(&ts.tv_sec, &t);

	memset(datetime, 0, sizeof(datetime));
	snprintf(datetime, sizeof(datetime), "%d-%02d-%02dT%02d:%02d:%02d",
			 t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
			 t.tm_sec);
	printf("current date time: %s\n", datetime);
}

void do_something()
{
	show_datetime();
}

int main()
{
	struct timespec ts1;
	struct timespec ts2;
	while (true) {
		timespec_get(&ts1, TIME_UTC);

		clear_screen();
		do_something();
		fflush(stdout);

		timespec_get(&ts2, TIME_UTC);
		uint64_t elapsed_ms = ((ts2.tv_sec - ts1.tv_sec) * 1000000 +
							   ts2.tv_nsec / 1000 - ts1.tv_nsec / 1000) /
							  1000;
		if (elapsed_ms < 16) {
			muggle_msleep(16 - elapsed_ms);
		}
	}

	return 0;
}
