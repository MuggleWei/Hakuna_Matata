#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

int main(int argc, char *argv[])
{
	muggle_log_complicated_init(-1, LOG_LEVEL_DEBUG, NULL);

	const char *term_type = getenv("TERM");
	LOG_INFO("TERM: %s", term_type);

	initscr();

	curs_set(0);
	keypad(stdscr, true);

	clear();
	noecho();
	cbreak();

	mousemask(ALL_MOUSE_EVENTS, NULL);

	int to_exit = 0;
	while (1)
	{
		int c = getch();
		switch (c)
		{
			case 'q':
			{
				to_exit = 1;
			}break;
			case '5':
			{
				LOG_INFO("5");
			}break;
			case KEY_MOUSE:
			{
				LOG_INFO("key mouse: %d", c);
				MEVENT event;
				if (getmouse(&event) == OK)
				{
					LOG_INFO("get mouse event: %d", event.bstate);
					if (event.bstate & BUTTON1_RELEASED)
					{
						clear();
						LOG_INFO("click x=%d, y=%d", event.x, event.y);
						mvprintw(LINES - 1, 0, "click x=%d, y=%d", event.x, event.y);
						refresh();
					}
				}
			}break;
			default:
			{
				LOG_INFO("getch: %d", c);
			}break;
		}

		if (to_exit)
		{
			break;
		}
	}

	endwin();
	
	return 0;
}
