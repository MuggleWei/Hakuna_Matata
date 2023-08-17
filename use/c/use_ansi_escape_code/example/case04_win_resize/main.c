#include <sys/ioctl.h>
#include <signal.h>
#include <stdint.h>
#include "muggle/c/muggle_c.h"

uint16_t win_col;
uint16_t win_row;

void on_win_resize(int signo)
{
	MUGGLE_UNUSED(signo);

	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	win_col = ws.ws_col;
	win_row = ws.ws_row;
}

void on_update()
{
	// do nothing
}

void on_render()
{
	fprintf(stdout, "\033[2J\033[1;1H");
	fprintf(stdout, "row=%u, col=%u\n", win_col, win_row);
}

void message_loop()
{
	struct timespec ts1;
	struct timespec ts2;
	while (true) {
		timespec_get(&ts1, TIME_UTC);

		on_update();

		on_render();

		timespec_get(&ts2, TIME_UTC);
		uint64_t elapsed_ms = ((ts2.tv_sec - ts1.tv_sec) * 1000000 +
							   ts2.tv_nsec / 1000 - ts1.tv_nsec / 1000) /
							  1000;
		if (elapsed_ms < 16) {
			muggle_msleep(16 - elapsed_ms);
		}
	}
}

int main()
{
	signal(SIGWINCH, on_win_resize);

	on_win_resize(SIGWINCH);
	on_render();

	message_loop();

	return 0;
}
