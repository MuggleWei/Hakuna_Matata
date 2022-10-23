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

void print_tip(int c)
{
	mvprintw(
		LINES - 1, 0,
		"Charcter pressed is = %3d, "
		"Hopefully it can be printed as '%c'", c, c);
}

void clear_tip()
{
	int y, x;
	getyx(stdscr, y, x); // save current pos
	move(LINES - 1, 0);  // move to tip line
	clrtoeol();          // clear line
	move(y, x);          // move back
}

int main()
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	initscr();

	curs_set(0);

	clear();
	noecho();
	cbreak();

	int startx = (COLS - WIDTH) / 2;
	int starty = (LINES - HEIGHT) / 2;

	WINDOW *menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(
		0, 0,
		"Use arrow keys to go up and down, Press enter to select a choice");
	refresh();

	int choice = 0;
	int highlight = 1;
	print_menu(menu_win, highlight);
	while (1)
	{
		int c = wgetch(menu_win);
		switch (c)
		{
			case KEY_UP:
			{
				if (highlight == 1)
				{
					highlight = n_choices;
				}
				else
				{
					--highlight;
				}
				clear_tip();
			}break;
			case KEY_DOWN:
			{
				if (highlight == n_choices)
				{
					highlight = 1;
				}
				else 
				{
					++highlight;
				}
				clear_tip();
			}break;
			case 10:
			{
				choice = highlight;
				clear_tip();
			}break;
			default:
			{
				print_tip(c);
			}break;
		}
		refresh();
		print_menu(menu_win, highlight);
		if (choice != 0)
		{
			break;
		}
	}

	mvprintw(
		LINES - 1, 0,
		"You chose choice %d with choices string %s",
		choice, choices[choice - 1]);
	getch();

	refresh();

	endwin();
	
	return 0;
}
