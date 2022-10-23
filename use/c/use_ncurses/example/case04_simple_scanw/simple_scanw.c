#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	const char *prompt = "Enter a string: ";
	int row = 0, col = 0;
	char str[80];

	initscr();

	getmaxyx(stdscr, row, col);

	mvprintw(row / 2, (col - strlen(prompt)) / 2, "%s", prompt);

	getstr(str);
	mvprintw(LINES - 2, 0, "You Entered: %s", str);

	getch();

	endwin();
	
	return 0;
}
