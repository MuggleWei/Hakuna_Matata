#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"

WINDOW* create_newwin(int height, int width, int starty, int startx)
{
	WINDOW *local_win = newwin(height, width, starty, startx);

	// 0, 0 gives default characters for the vertical and horizontal lines
	box(local_win, 0, 0);
	wborder(local_win,
		'|', '|', '-', '-',
		'+', '+', '+', '+');

	// show that box
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	// The parameters taken are 
	// 1. win: the window on which to operate
	// 2. ls: character to be used for the left side of the window 
	// 3. rs: character to be used for the right side of the window 
	// 4. ts: character to be used for the top side of the window 
	// 5. bs: character to be used for the bottom side of the window 
	// 6. tl: character to be used for the top left corner of the window 
	// 7. tr: character to be used for the top right corner of the window 
	// 8. bl: character to be used for the bottom left corner of the window 
	// 9. br: character to be used for the bottom right corner of the window
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

	wrefresh(local_win);
	delwin(local_win);
}

int main(int argc, char *argv[])
{
	initscr();

	cbreak();
	keypad(stdscr, true);

	int height = 3;
	int width = 10;
	int starty = (LINES - height) / 2;
	int startx = (COLS - width) / 2;
	printw("Press F1 to exit");
	refresh();

	WINDOW *my_win = create_newwin(height, width, starty, startx);
	int ch = EOF;
	while ((ch = getch()) != KEY_F(1))
	{
		switch (ch)
		{
			case KEY_LEFT:
			{
				destroy_win(my_win);
				my_win = create_newwin(height, width, starty, --startx);
			}break;
			case KEY_RIGHT:
			{
				destroy_win(my_win);
				my_win = create_newwin(height, width, starty, ++startx);
			}break;
			case KEY_UP:
			{
				destroy_win(my_win);
				my_win = create_newwin(height, width, --starty, startx);
			}break;
			case KEY_DOWN:
			{
				destroy_win(my_win);
				my_win = create_newwin(height, width, ++starty, startx);
			}break;
		}
	}

	endwin();
	
	return 0;
}
