#ifndef _IO_H
#define _IO_H

//key codes
#define ESC 27
#define CTRL_C 3
#ifdef __WIN32__
#define KP_DEL 78
#else
#define KP_DEL 74
#endif

typedef enum {
	CONTINUE,
	LOST,
	WON
}status;

void draw_screen();
status handle_input(int *key,int *y,int *x, int level);
int init_curses();
void print_info(int errs,int level);

#endif
