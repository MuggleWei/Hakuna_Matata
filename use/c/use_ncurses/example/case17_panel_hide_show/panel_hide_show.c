#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"
#include "ncurses/panel.h"

typedef struct _PANEL_DATA
{
	int hide; /* TRUE if panel is hidden */
} PANEL_DATA;

void init_wins(WINDOW **wins, int n);
void win_show(WINDOW *win, char *label, int label_color);
void print_in_middle(
	WINDOW *win, int starty, int startx, int width, char *string, chtype color);

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	clear();
	start_color();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);

	WINDOW *my_wins[3];
	init_wins(my_wins, 3);

	PANEL *my_panels[3];
	for (int i = 0; i < 3; i++)
	{
		my_panels[i] = new_panel(my_wins[i]);
	}

	PANEL_DATA panel_datas[3];
	for (int i = 0; i < 3; i++)
	{
		panel_datas[i].hide = FALSE;
		set_panel_userptr(my_panels[i], &panel_datas[i]);
	}

	update_panels();

	attron(COLOR_PAIR(4));
	mvprintw(LINES - 3, 0, "Show or Hide a window with 'a', 'b', 'c'");
	mvprintw(LINES - 2, 0, "F1 to Exit");
	attroff(COLOR_PAIR(4));

	doupdate();

	int ch;
	while ((ch = getch()) != KEY_F(1))
	{
		int show_hide_idx = -1;
		switch (ch)
		{
		case 'a':
		{
			show_hide_idx = 0;
		}
		break;
		case 'b':
		{
			show_hide_idx = 1;
		}
		break;
		case 'c':
		{
			show_hide_idx = 2;
		}
		break;
		}

		if (show_hide_idx >= 0)
		{
			PANEL_DATA *tmp =
				(PANEL_DATA *)panel_userptr(my_panels[show_hide_idx]);
			if (tmp->hide)
			{
				tmp->hide = FALSE;
				show_panel(my_panels[show_hide_idx]);
			}
			else
			{
				tmp->hide = TRUE;
				hide_panel(my_panels[show_hide_idx]);
			}
			update_panels();
			doupdate();
		}
	}

	endwin();

	return 0;
}

void init_wins(WINDOW **wins, int n)
{
	char label[80];

	int y = 2;
	int x = 10;
	for (int i = 0; i < n; ++i)
	{
		wins[i] = newwin(10, 40, y, x);
		sprintf(label, "Window Number %d", i + 1);
		win_show(wins[i], label, i + 1);
		y += 3;
		x += 7;
	}
}

void win_show(WINDOW *win, char *label, int label_color)
{
	int startx, starty, height, width;

	getbegyx(win, starty, startx);
	getmaxyx(win, height, width);

	box(win, 0, 0);
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwhline(win, 2, 1, ACS_HLINE, width - 2);
	mvwaddch(win, 2, width - 1, ACS_RTEE);

	print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

void print_in_middle(
	WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
	int length, x, y;

	if (win == NULL)
	{
		win = stdscr;
	}

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
		width = 80;
	}

	int len = strlen(string);
	int tmp = (width - len) / 2;

	x = startx + (int)tmp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}
