#include "ncurses/ncurses.h"
#include "muggle/c/muggle_c.h"

int main(int argc, char *argv[])
{
	/* initialize log */
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	if (argc != 2)
	{
		LOG_ERROR("Usage: %s <a c file>", argv[0]);
		exit(EXIT_FAILURE);
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		LOG_ERROR("Can't open input file: %s", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Start curses mode */
	initscr();

	int row, col;
	getmaxyx(stdscr, row, col);

	int ch, prev = EOF;
	int x, y;
	while ((ch = fgetc(fp)) != EOF)  /* read the file till we reach the end */
	{
		/* get the current cursor position */
		getyx(stdscr, y, x);

		/* are we are at the end of the screen */
		if (y == (row - 1))
		{
			printw("<-Press Any Key->");  /* tell the user to press a key */
			getch();
			clear();
			move(0, 0);
		}

		if (prev == '/' && ch == '*')
		{
			attron(A_BOLD);
			getyx(stdscr, y, x);
			move(y, x - 1);
			printw("%c%c", '/', ch);
		}
		else
		{
			printw("%c", ch);
		}

		refresh();

		if (prev == '*' && ch == '/')
		{
			attroff(A_BOLD);
		}

		prev = ch;
	}

	getch();

	/* End curses mode */
	endwin();

	fclose(fp);

	return 0;
}
