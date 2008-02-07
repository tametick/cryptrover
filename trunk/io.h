#ifndef _IO_H
#define _IO_H

//key codes
#define ESC 27
#define CTRL_C 3
#ifdef __WIN32__
#define KP_DEL 78
#else
#define KP_DEL 74
#define KP_5 69
#endif

void add_message(char *msg,attr_t attr);
void draw_screen(void);
int init_curses(void);
void init_message_win(int info_lines);
int print_info(int errs,int level);
int readchar(void);

#endif
