#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"
#include "ncurses/panel.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	WINDOW *my_wins[3];
	PANEL *my_panels[3];

	int lines = 10, cols = 40;
	int y = 2, x = 4;

	initscr();

	clear();
	noecho();
	cbreak();

	// create windows for the panels
	my_wins[0] = newwin(lines, cols, y, x);
	my_wins[1] = newwin(lines, cols, y + 1, x + 5);
	my_wins[2] = newwin(lines, cols, y + 2, x + 10);
	for (int i = 0; i < 3; i++)
	{
		box(my_wins[i], 0, 0);
	}

	// attach a panel to each window, order is bottom up
	for (int i = 0; i < 3; i++)
	{
		my_panels[i] = new_panel(my_wins[i]);
	}

	// update the stacking order, 2nd panel will be on top
	update_panels();

	// show it on the screen
	doupdate();

	getch();
	endwin();
	
	return 0;
}
