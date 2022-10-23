#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

typedef struct
{
	chtype ls;
	chtype rs;
	chtype ts;
	chtype bs;
	chtype tl;
	chtype tr;
	chtype bl;
	chtype br;
} win_border_t;

typedef struct
{
	int startx;
	int starty;
	int height;
	int width;
	win_border_t border;
} win_t;

void init_win_params(win_t *p_win)
{
	p_win->height = 3;
	p_win->width = 10;
	p_win->starty = (LINES - p_win->height) / 2;
	p_win->startx = (COLS - p_win->width) / 2;

	p_win->border.ls = '|';
	p_win->border.rs = '|';
	p_win->border.ts = '-';
	p_win->border.bs = '-';
	p_win->border.tl = '+';
	p_win->border.tr = '+';
	p_win->border.bl = '+';
	p_win->border.br = '+';
}

void print_win_params(win_t *p_win)
{
	LOG_DEBUG("win: startx=%d, starty=%d, width=%d, height=%d",
		p_win->startx, p_win->starty, p_win->width, p_win->height);
}

void create_box(win_t *p_win, bool flag)
{
	int x = p_win->startx;
	int y = p_win->starty;
	int w = p_win->width;
	int h = p_win->height;

	if (flag)
	{
		mvaddch(y,     x,     p_win->border.tl);
		mvaddch(y,     x + w, p_win->border.tr);
		mvaddch(y + h, x,     p_win->border.bl);
		mvaddch(y + h, x + w, p_win->border.br);

		mvhline(y,     x + 1, p_win->border.ts, w - 1);
		mvhline(y + h, x + 1, p_win->border.bs, w - 1);
		mvvline(y + 1, x,     p_win->border.ls, h - 1);
		mvvline(y + 1, x + w, p_win->border.rs, h - 1);
	}
	else
	{
		for (int j = y; j <= y + h; ++j)
		{
			for (int i = x; i <= x + w; ++i)
			{
				mvaddch(j, i, ' ');
			}
		}
	}

	refresh();
}

int main(int argc, char *argv[])
{
	// initialize log
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	// start curses mode
	initscr();

	start_color();
	cbreak();
	keypad(stdscr, true);
	noecho();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);

	// initialize the window parameters
	win_t win;
	init_win_params(&win);
	print_win_params(&win);

	attron(COLOR_PAIR(1));
	printw("Press F1 to exit");
	refresh();
	attroff(COLOR_PAIR(1));

	create_box(&win, true);
	int ch;
	while ((ch = getch()) != KEY_F(1))
	{
		switch (ch)
		{
			case KEY_LEFT:
			{
				create_box(&win, false);
				--win.startx;
				create_box(&win, TRUE);
			}break;
			case KEY_RIGHT:
			{
				create_box(&win, false);
				++win.startx;
				create_box(&win, TRUE);
			}break;
			case KEY_UP:
			{
				create_box(&win, false);
				--win.starty;
				create_box(&win, TRUE);
			}break;
			case KEY_DOWN:
			{
				create_box(&win, false);
				++win.starty;
				create_box(&win, TRUE);
			}break;
		}
	}

	// end curses mode
	endwin();
	
	return 0;
}
