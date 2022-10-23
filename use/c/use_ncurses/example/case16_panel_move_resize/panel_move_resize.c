#include "muggle/c/muggle_c.h"
#include "ncurses/ncurses.h"
#include "ncurses/panel.h"

typedef struct _PANEL_DATA
{
	int x, y, w, h;
	char label[80]; 
	int label_color;
	PANEL *next;
}PANEL_DATA;

void init_wins(WINDOW **wins, int n);
void win_show(WINDOW *win, char *label, int label_color);
void print_in_middle(
	WINDOW *win,
	int starty, int startx, int width, char *string, chtype color);
void set_user_ptrs(PANEL **panels, int n);
void my_move_panel(PANEL *panel, int y, int x);
void my_resize_panel(PANEL *panel, int h, int w, int y, int x);

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

	set_user_ptrs(my_panels, 3);

	update_panels();

	attron(COLOR_PAIR(4));
	mvprintw(LINES - 3, 0, "Use 'm' for moving, 'r' for resizing");
	mvprintw(
		LINES - 2, 0, "Use tab to browse through the windows (F1 to Exit)");
	attroff(COLOR_PAIR(4));
	doupdate();

	PANEL *stack_top = my_panels[2];
	PANEL_DATA *top = (PANEL_DATA *)panel_userptr(stack_top);
	int newx = top->x;
	int newy = top->y;
	int neww = top->w;
	int newh = top->h;

	int ch;
	int size = FALSE;
	int move = FALSE;
	while((ch = getch()) != KEY_F(1))
	{
		switch(ch)
		{
			case 9: // Tab
			{
				top = (PANEL_DATA *)panel_userptr(stack_top);
				top_panel(top->next);
				stack_top = top->next;
				top = (PANEL_DATA *)panel_userptr(stack_top);
				newx = top->x;
				newy = top->y;
				neww = top->w;
				newh = top->h;
			}break;
			case 'r': // Resize
			{
				move = FALSE;
				size = TRUE;

				attron(COLOR_PAIR(4));
				mvprintw(
					LINES - 4, 0,
					"Entered Resizing: "
					"Use Arrow Keys to resize and "
					"press <ENTER> to end resizing");
				refresh();
				attroff(COLOR_PAIR(4));
			}break;
			case 'm': // Move
			{
				move = TRUE;
				size = FALSE;

				attron(COLOR_PAIR(4));
				mvprintw(
					LINES - 4, 0,
					"Entered Moving: "
					"Use Arrow Keys to Move and press <ENTER> to end moving");
				refresh();
				attroff(COLOR_PAIR(4));
			}break;
			case KEY_LEFT:
			{
				if(size == TRUE)
				{
					--newx;
					++neww;
					my_resize_panel(stack_top, newh, neww, newy, newx);
				}
				if(move == TRUE)
				{
					--newx;
					my_move_panel(stack_top, newy, newx);
				}
			}break;
			case KEY_RIGHT:
			{
				if(size == TRUE)
				{
					++newx;
					--neww;
					my_resize_panel(stack_top, newh, neww, newy, newx);
				}
				if(move == TRUE)
				{
					++newx;
					my_move_panel(stack_top, newy, newx);
				}
			}break;
			case KEY_UP:
			{
				if(size == TRUE)
				{
					--newy;
					++newh;
					my_resize_panel(stack_top, newh, neww, newy, newx);
				}
				if(move == TRUE)
				{
					--newy;
					my_move_panel(stack_top, newy, newx);
				}
			}break;
			case KEY_DOWN:
			{
				if(size == TRUE)
				{
					++newy;
					--newh;
					my_resize_panel(stack_top, newh, neww, newy, newx);
				}
				if(move == TRUE)
				{
					++newy;
					my_move_panel(stack_top, newy, newx);
				}
			}break;
			case 10: // Enter
			{
				move(LINES - 4, 0);
				clrtoeol();
				refresh();
				if(size == TRUE)
				{
					size = FALSE;
				}
				if(move == TRUE)
				{
					move = FALSE;
				}
			}break;
		}
		attron(COLOR_PAIR(4));
		mvprintw(LINES - 3, 0, "Use 'm' for moving, 'r' for resizing");
		mvprintw(
			LINES - 2, 0,
			"Use tab to browse through the windows (F1 to Exit)");
		attroff(COLOR_PAIR(4));
		refresh();	
		update_panels();
		doupdate();
	}

	endwin();

	return 0;
}

void init_wins(WINDOW **wins, int n)
{
	char label[80];

	int y = 2;
	int x = 10;
	for(int i = 0; i < n; ++i)
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
	WINDOW *win,
	int starty, int startx, int width, char *string, chtype color)
{
	int length, x, y;

	if(win == NULL)
	{
		win = stdscr;
	}

	getyx(win, y, x);
	if(startx != 0)
	{
		x = startx;
	}
	if(starty != 0)
	{
		y = starty;
	}
	if(width == 0)
	{
		width = 80;
	}

	int len = strlen(string);
	int tmp = (width - len)/ 2;
	x = startx + (int)tmp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

void set_user_ptrs(PANEL **panels, int n)
{
	PANEL_DATA *ptrs;
	WINDOW *win;
	int x, y, w, h, i;
	char temp[80];
	
	ptrs = (PANEL_DATA *)calloc(n, sizeof(PANEL_DATA));

	for(i = 0;i < n; ++i)
	{
		win = panel_window(panels[i]);
		getbegyx(win, y, x);
		getmaxyx(win, h, w);
		ptrs[i].x = x;
		ptrs[i].y = y;
		ptrs[i].w = w;
		ptrs[i].h = h;
		sprintf(temp, "Window Number %d", i + 1);
		strcpy(ptrs[i].label, temp);
		ptrs[i].label_color = i + 1;
		if(i + 1 == n)
			ptrs[i].next = panels[0];
		else
			ptrs[i].next = panels[i + 1];
		set_panel_userptr(panels[i], &ptrs[i]);
	}
}

void my_move_panel(PANEL *panel, int y, int x)
{
	move_panel(panel, y, x);

	PANEL_DATA *panel_data = (PANEL_DATA *)panel_userptr(panel);
	panel_data->y = y;
	panel_data->x = x;
}

void my_resize_panel(PANEL *panel, int h, int w, int y, int x)
{
	if (y < 0 || x < 0)
	{
		return;
	}

	WINDOW *old_win = panel_window(panel);
	WINDOW *tmp_win = newwin(h, w, y, x);
	replace_panel(panel, tmp_win);
	PANEL_DATA *panel_data = (PANEL_DATA *)panel_userptr(panel);
	win_show(tmp_win, panel_data->label, panel_data->label_color); 
	delwin(old_win);

	panel_data->h = h;
	panel_data->w = w;
	panel_data->y = y;
	panel_data->x = x;
}
