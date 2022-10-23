#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"
#include "ncurses/panel.h"

void print_in_middle(
	WINDOW *win, int starty, int startx, int width, char *str, chtype color)
{
	if (win == NULL)
	{
		win = stdscr;
	}

	int y = 0, x = 0;
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

	int len = strlen(str);
	int tmp = (width - len) / 2;
	x = startx + (int)tmp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", str);
	wattroff(win, color);
	refresh();
}

void win_show(WINDOW *win, char *label, int label_color)
{
	int startx, starty;
	int height, width;

	getbegyx(win, starty, startx);
	getmaxyx(win, height, width);

	box(win, 0, 0);
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwhline(win, 2, 1, ACS_HLINE, width - 2);
	mvwaddch(win, 2, width - 1, ACS_RTEE);

	print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

void init_wins(WINDOW **wins, int n)
{
	int y = 2;
	int x = 10;
	char label[80];
	for (int i = 0; i < n; ++i)
	{
		wins[i] = newwin(10, 40, y, x);
		snprintf(label, sizeof(label), "Window Number %d", i + 1);
		win_show(wins[i], label, i + 1);
		y += 3;
		x += 7;
	}
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	start_color();
	clear();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	// initialize all the colors
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_BLACK);

	WINDOW *my_wins[3];
	init_wins(my_wins, 3);

	// attach panel to each window
	PANEL *my_panels[3];
	for (int i = 0; i < 3; i++)
	{
		my_panels[i] = new_panel(my_wins[i]);
	}

	// setup the user pointers to the next panel
	set_panel_userptr(my_panels[0], my_panels[1]);
	set_panel_userptr(my_panels[1], my_panels[2]);
	set_panel_userptr(my_panels[2], my_panels[0]);

	// update the stacking order. 2nd panel will be on top
	update_panels();

	// show it on the screen
	attron(COLOR_PAIR(4));
	mvprintw(
		LINES - 2, 0,
		"Use tab to browse through the windows (F1 to Exit)");
	attroff(COLOR_PAIR(4));
	doupdate();

	PANEL *top = my_panels[2];
	int ch;
	while ((ch = getch()) != KEY_F(1))
	{
		switch (ch)
		{
			case 9:
			{
				top = (PANEL*)panel_userptr(top);
				top_panel(top);
			}break;
		}
		update_panels();
		doupdate();
	}

	endwin();
	
	return 0;
}
