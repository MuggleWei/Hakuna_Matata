#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	// start curses mode
	initscr();

	// start color functionality
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);

	printw("A Big string which I didn' care to type fully");

	// First two parameters specify the position at which to start.
	// Third parameter number of characters to update. -1 means till end of 
	// line.
	// Forth parameter is the normal attribute you wanted to give to the 
	// charcter.
	// Fifth is the color index. It's the index given during init_pair(), use
	// 0 if you didn't want color.
	// Sixth one is always NULL
	mvchgat(0, 0, -1, A_BLINK, 1, NULL);

	move(1, 0);
	printw("hello world");

	refresh();

	// wait exit
	getch();

	// end curses mode
	endwin();
	
	return 0;
}
