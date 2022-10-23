#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

const char *items[] = {
	"c",
	"python",
	"c++",
	"go",
	"Java",
	"c#",
	"rust",
};
static const int n_item = sizeof(items) / sizeof(items[0]);

void show_menu(WINDOW *menu_win, int selected)
{
	int x = 2;
	int y = 1;
	box(menu_win, 0, 0);
	for (int i = 0; i < n_item; i++)
	{
		if (selected == i)
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", items[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
		{
			mvwprintw(menu_win, y, x, "%s", items[i]);
		}
		++y;
	}
	wrefresh(menu_win);
}

void show_selected(int selected)
{
	int x, y;
	getyx(stdscr, y, x);
	move(LINES - 1, 0);
	clrtoeol();

	if (selected >= 0 && selected < n_item)
	{
		mvprintw(LINES - 1, 0, "selected: %s", items[selected]);
	}

	move(y, x);
}

int main()
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	curs_set(0);
	keypad(stdscr, TRUE);

	clear();
	noecho();
	cbreak();

	mousemask(ALL_MOUSE_EVENTS, NULL);

	int menu_width = 20;
	int menu_height = n_item + 2;
	int startx = (COLS - menu_width) / 2;
	int starty = (LINES - menu_height) / 2;
	WINDOW *menu_win = newwin(menu_height, menu_width, starty, startx);
	keypad(menu_win, TRUE);

	int selected = -1;
	show_menu(menu_win, selected);

	int should_exit = 0;
	while (1)
	{
		int c = wgetch(menu_win);
		switch (c)
		{
			case KEY_UP:
			{
				if (selected == -1)
				{
					selected = 0;
				}
				else
				{
					--selected;
					if (selected == -1)
					{
						selected = n_item - 1;
					}
				}
				LOG_DEBUG("selected: %d", selected);
				show_menu(menu_win, selected);
			}break;
			case KEY_DOWN:
			{
				if (selected == -1)
				{
					selected = 0;
				}
				else
				{
					++selected;
					if (selected >= n_item)
					{
						selected = 0;
					}
				}
				LOG_DEBUG("selected: %d", selected);
				show_menu(menu_win, selected);
			}break;
			case KEY_MOUSE:
			{
				LOG_DEBUG("key mouse");
				MEVENT mouse_event;
				if (getmouse(&mouse_event) != OK)
				{
					break;
				}

				if (mouse_event.bstate & BUTTON1_RELEASED ||
					mouse_event.bstate & BUTTON1_CLICKED ||
					mouse_event.bstate & BUTTON1_DOUBLE_CLICKED)
				{
					int mouse_menu_x = mouse_event.x - startx - 1;
					int mouse_menu_y = mouse_event.y - starty - 1;
					LOG_DEBUG(
						"mouse selected: "
						"x=%d, y=%d, rel_x=%d, rel_y=%d",
						mouse_event.x, mouse_event.y,
						mouse_menu_x, mouse_menu_y);
					if (mouse_menu_y < 0 || mouse_menu_y >= n_item)
					{
						selected = -1;
					}
					else
					{
						if (mouse_menu_x >= 0 &&
							mouse_menu_x <= menu_width)
						{
							selected = mouse_menu_y;
						}
						else
						{
							selected = -1;
						}
					}

					if (selected == -1)
					{
						show_selected(selected);
						refresh();
					}
					else if (mouse_event.bstate & BUTTON1_DOUBLE_CLICKED)
					{
						show_selected(selected);
						refresh();
					}
					show_menu(menu_win, selected);
				}
			}break;
			case 'q':
			{
				should_exit = 1;
			}break;
			case '\r':
			case '\n':
			{
				show_selected(selected);
				refresh();
				show_menu(menu_win, selected);
			}break;
		}

		if (should_exit)
		{
			break;
		}
	}
	
	// printf("\033[?1003l\n"); // Disable mouse movement events, as l = low
	endwin();

	return 0;
}
