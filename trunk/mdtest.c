#ifdef __WIN32__
#include "pdc28_ming_w32/curses.h"
#else
#include <ncurses.h>
#endif

#include <stdbool.h>
#include "mdport.h"

int errs;
void init_curses() {
	errs=0;
	if (ERR==keypad(stdscr=initscr(),true))
		mvaddstr(errs++,0,"Cannot enable keypad");
	if (ERR==noecho())
		mvaddstr(errs++,0,"Cannot set noecho mode");
	if (ERR==curs_set(0))
		mvaddstr(errs++,0,"Cannot set invisible cursor");
	if (ERR==start_color() || !has_colors())
		mvaddstr(errs++,0,"Cannot enable colors");
	else {
		for (int c=0; c<8; c++)
			init_pair(c,c,0);
	}
}
void print_info(int c) {
	int msgs=errs;
	char msg[50];
	sprintf(msg, "c: %d     ",c);
	mvaddstr(msgs++,0,msg);
}
int main () {
	init_curses();
	int c;
	while (c!=3) {
		print_info(c=md_readchar(stdscr));
	}
	return endwin();

}
