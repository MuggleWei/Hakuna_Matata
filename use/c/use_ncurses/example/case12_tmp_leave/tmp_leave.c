#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	printw("hello world!!!\n");
	refresh();

	getch();

	def_prog_mode();   // Save the tty modes
	endwin();          // End curses mode temporarily
	system("/bin/sh"); // Do whatever you like in cooked mode

	reset_prog_mode(); // Return to the previous tty mode
	refresh();

	printw("Another String\n"); // Back to curses use the full
	refresh();

	getch();
	endwin();
	
	return 0;
}
