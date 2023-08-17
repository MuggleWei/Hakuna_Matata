#include <sys/ioctl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include "muggle/c/muggle_c.h"

#define ANSI_ESC "\x1B"
#define ANSI_CSI ANSI_ESC "["

// alternate buffer
#define ANSI_ALTER_BUF_ON ANSI_CSI "?1049h"
#define ANSI_ALTER_BUF_OFF ANSI_CSI "?1049l"

// cursor
#define ANSI_CURSOR_ON ANSI_CSI "?25h"
#define ANSI_CURSOR_OFF ANSI_CSI "?25l"

// clear the screen and nukes scrollback
#define ANSI_CLEAN_SCREEN ANSI_CSI "2J"

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

void cleanup()
{
	// clean up the alternate buffer
	fprintf(stdout, ANSI_CLEAN_SCREEN);

	// switch back to normal buffer
	fprintf(stdout, ANSI_ALTER_BUF_OFF);

	// show the cursor again
	fprintf(stdout, ANSI_CURSOR_ON);

	fflush(stdout);
}

void cleanup_die(int signo)
{
	MUGGLE_UNUSED(signo);
	exit(1);
}

void move_to(int16_t row, int16_t col)
{
	fprintf(stdout, ANSI_CSI "%u;%uH", row, col);
}

void on_update()
{
	// do nothing
}

void on_render()
{
	fprintf(stdout, ANSI_CLEAN_SCREEN);

	const char *msg = "hello world";

	// move to
	uint16_t r = win_row / 2;
	uint16_t c = win_col / 2 - strlen(msg) / 2;
	move_to(r, c);

	fprintf(stdout, "hello world");

	fflush(stdout);
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
	// win resize callback
	signal(SIGWINCH, on_win_resize);

	// enter the alternate buffer
	fprintf(stdout, ANSI_ALTER_BUF_ON);

	// register our cleanup function
	atexit(cleanup);
	signal(SIGTERM, cleanup_die);
	signal(SIGINT, cleanup_die);

	// clean up the buffer
	fprintf(stdout, ANSI_CLEAN_SCREEN);

	// hide the cursor
	fprintf(stdout, ANSI_CURSOR_OFF);

	fflush(stdout);

	on_win_resize(SIGWINCH);

	message_loop();

	return 0;
}

