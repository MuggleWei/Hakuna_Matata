#include "muggle/c/muggle_c.h"
#include "ncurses/menu.h"
#include "ncurses/ncurses.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

const char *choices[] = {
	"Choice 1", "Choice 2", "Choice 3", "Choice 4", "Exit",
};

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	clear();
	start_color();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	int n_choices = ARRAY_SIZE(choices);
	ITEM **my_items = (ITEM **)malloc((n_choices + 1) * sizeof(ITEM *));
	for (int i = 0; i < n_choices; ++i)
	{
		my_items[i] = new_item(choices[i], choices[i]);
	}
	my_items[n_choices] = (ITEM *)NULL;

	MENU *my_menu = new_menu((ITEM **)my_items);
	mvprintw(LINES - 2, 0, "F1 to Exit");
	post_menu(my_menu);
	refresh();

	int c;
	while ((c = getch()) != KEY_F(1))
	{
		switch (c)
		{
		case KEY_DOWN:
		{
			menu_driver(my_menu, REQ_DOWN_ITEM);
		}
		break;
		case KEY_UP:
		{
			menu_driver(my_menu, REQ_UP_ITEM);
		}
		break;
		}
	}

	for (int i = 0; i < n_choices; ++i)
	{
		free_item(my_items[i]);
	}
	free_menu(my_menu);

	endwin();

	return 0;
}
