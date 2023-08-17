#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <signal.h>
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

int main()
{
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

	fprintf(stdout, "sleep for 2000 ms\n");
	fflush(stdout);
	muggle_msleep(2000);

	return 0;
}
