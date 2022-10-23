#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *str)
{
	if (win == NULL)
	{
		win = stdscr;
	}

	int x, y;
	getyx(win, y, x);

	if (startx != 0)
	{
		x = startx;
	}
	if (starty != 0)
	{
		y = starty;
	}
	if (width == 0)
	{
		width = COLS;
	}

	int length = strlen(str);
	int offset = (width - length) / 2;
	x = startx + offset;

	mvwprintw(win, y, x, "%s", str);
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	// start curses mode
	initscr();

	if (!has_colors())
	{
		endwin();
		LOG_ERROR("Your terminal does not support color");
		exit(EXIT_FAILURE);
	}

	if (can_change_color())
	{
		init_color(COLOR_RED, 700, 0, 0);
	}

	start_color();

	init_pair(1, COLOR_RED, COLOR_BLACK);

	attron(COLOR_PAIR(1));
	print_in_middle(stdscr, LINES / 2, 0, 0, "Viola!!! In color ...");
	attroff(COLOR_PAIR(1));

	refresh();
	getch();

	// end curses mode
	endwin();

	return 0;
}
