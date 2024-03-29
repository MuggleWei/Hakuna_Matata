#include <ncurses.h>

int main(int argc, char *argv[])
{
	// start curses mode
	initscr();

	// line buffering disabled
	raw();

	// We get F1, F2 etc...
	keypad(stdscr, true);

	// don't echo() while we do getch
	noecho();

	printw("Type any character to see it in bold\n");

	int ch = getch();
	if (ch == KEY_F(1))
	{
		printw("F1 Key pressed");
	}
	else
	{
		printw("The pressed key is: ");
		attron(A_BOLD);
		printw("%c", ch);
		attroff(A_BOLD);
	}

	refresh();

	getch();
	endwin();

	return 0;
}
