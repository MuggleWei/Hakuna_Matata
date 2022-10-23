#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	curs_set(0);

	const char *msg = "Just Play";

	while (1)
	{
		clear();

		int row, col;
		getmaxyx(stdscr, row, col);
		mvprintw(row / 2, (col - strlen(msg)) / 2, "%s", msg);

		mvprintw(row - 2, 0, "This screen has %d rows and %d columns\n", row, col);
		printw("Try resizing your window(if possible) and then run this program again");

		refresh();

		int ch = getch();
		if (ch == 'q')
		{
			break;
		}

		LOG_INFO("recv %d", ch);
	}

	endwin();
	
	return 0;
}
