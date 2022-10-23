#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

#define WIDTH  30
#define HEIGHT 10

const char *choices[] = {
	"Item 1",
	"Item 2",
	"Item 3",
	"Item 4",
	"Exit"
};
static const int n_choices = sizeof(choices) / sizeof(choices[0]);

int startx = 0;
int starty = 0;

void print_menu(WINDOW *menu_win, int highlight)
{
	int x = 2;
	int y = 2;
	box(menu_win, 0, 0);
	for (int i = 0; i < n_choices; i++)
	{
		if (highlight == i + 1)
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
		{
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		}
		++y;
	}
	wrefresh(menu_win);
}

void report_choice(int mouse_x, int mouse_y, int *p_choice)
{
	int i = startx + 2;
	int j = starty + 3;

	for (int choice = 0; choice < n_choices; ++choice)
	{
		if (mouse_y == j + choice &&
			mouse_x >= i && mouse_x <= i + strlen(choices[choice]))
		{
			if (choice == n_choices - 1)
			{
				*p_choice = -1;
			}
			else
			{
				*p_choice = choice + 1;
			}
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	curs_set(0);
	keypad(stdscr, TRUE);

	clear();
	noecho();
	cbreak();

	mousemask(ALL_MOUSE_EVENTS, NULL);
	// mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	// printf("\033[?1003h\n"); // Makes the terminal report mouse movement events

	startx = (COLS - WIDTH) / 2;
	starty = (LINES - HEIGHT) / 2;

	WINDOW *menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	int highlight = 1;
	print_menu(menu_win, highlight);

	int choice = 1;
	while (1)
	{
		int c = wgetch(menu_win);
		switch (c)
		{
			case KEY_MOUSE:
			{
				MEVENT event;
				if (getmouse(&event) == OK)
				{
					if (event.bstate & BUTTON1_CLICKED)
					{
						report_choice(event.x + 1, event.y + 1, &choice);
						if (choice == -1)
						{
							goto end;
						}
						mvprintw(
							LINES - 1, 1, "Choice made is %d | \"%10s\"",
							choice, choices[choice - 1]);
						print_menu(menu_win, choice);
						refresh();
					}
				}
			}break;
			case 'q':
			{
				goto end;
			}break;
			default:
			{
				LOG_INFO("default: %d", c);
			}break;
		}
	}

end:
	// printf("\033[?1003l\n"); // Disable mouse movement events, as l = low
	endwin();
	
	return 0;
}
