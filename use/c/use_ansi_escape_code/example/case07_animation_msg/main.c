#include <sys/ioctl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
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

#define ANSI_COLOR_RGB ANSI_CSI "38;2;"

uint16_t win_col;
uint16_t win_row;

struct animation_msg {
	int16_t velocity_y;
	int16_t velocity_x;
	int16_t pos_y;
	int16_t pos_x;
	uint64_t rhythm;
	const char *msg;
	int32_t len;
	struct timespec ts;
};

struct animation_msg g_msg;

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

void update_color()
{
	int r = rand() % 256;
	int g = rand() % 256;
	int b = rand() % 256;
	fprintf(stdout, ANSI_COLOR_RGB "%d;%d;%dm", r, g, b);
}

void on_update()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	uint64_t elpased_ms = (ts.tv_sec - g_msg.ts.tv_sec) * 1000 +
						  ts.tv_nsec / 1000000 - g_msg.ts.tv_nsec / 1000000;
	if (elpased_ms < g_msg.rhythm) {
		return;
	}
	memcpy(&g_msg.ts, &ts, sizeof(g_msg.ts));

	int16_t offset_y = (elpased_ms / g_msg.rhythm) * g_msg.velocity_y;
	int16_t offset_x = (elpased_ms / g_msg.rhythm) * g_msg.velocity_x;
	g_msg.pos_y += offset_y;
	g_msg.pos_x += offset_x;

	if (g_msg.pos_y >= win_row) {
		g_msg.pos_y = win_row;
		g_msg.velocity_y *= -1;
		update_color();
	} else if (g_msg.pos_y <= 1) {
		g_msg.pos_y = 1;
		g_msg.velocity_y *= -1;
		update_color();
	}

	if (g_msg.pos_x + g_msg.len >= win_col) {
		g_msg.pos_x = win_col - g_msg.len;
		g_msg.velocity_x *= -1;
		update_color();
	} else if (g_msg.pos_x <= 1) {
		g_msg.pos_x = 1;
		g_msg.velocity_x *= -1;
		update_color();
	}
}

void on_render()
{
	fprintf(stdout, ANSI_CLEAN_SCREEN);

	move_to(g_msg.pos_y, g_msg.pos_x);
	fprintf(stdout, "%s", g_msg.msg);

	move_to(win_row, 0);
	printf("col=%d, row=%d", g_msg.pos_x, g_msg.pos_y);

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

	memset(&g_msg, 0, sizeof(g_msg));
	srand(time(NULL));
	g_msg.pos_x = rand() % win_col;
	g_msg.pos_y = rand() % win_row;
	g_msg.velocity_x = rand() % 2;
	if (g_msg.velocity_x == 0) {
		g_msg.velocity_x = -1;
	}
	g_msg.velocity_y = rand() % 2;
	if (g_msg.velocity_y == 0) {
		g_msg.velocity_y = -1;
	}
	g_msg.rhythm = 100;
	g_msg.msg = "hello world";
	g_msg.len = strlen(g_msg.msg);
	timespec_get(&g_msg.ts, TIME_UTC);

	message_loop();

	return 0;
}
