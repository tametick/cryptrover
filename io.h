#ifndef _IO_H
#define _IO_H

#ifdef __WIN32__
#include "pdc33_ming_w32\panel.h"
#else
#include <panel.h>
#endif

//key codes
#define ESC 27
#define CTRL_C 3
#ifdef __WIN32__
#define KP_DEL 78
#else
#define KP_DEL 74
#define KP_5 69
#endif

//colors
#define C_FOG COLOR_PAIR(8)|A_BOLD //fog of war
#define C_MED COLOR_PAIR(COLOR_GREEN)
#define C_AIR COLOR_PAIR(COLOR_BLUE)
#define C_BAT COLOR_PAIR(COLOR_YELLOW)

void add_message(char *msg,attr_t attr);
void draw_screen(void);
int end_curses(void);
int init_curses(void);
void init_message_win(int info_lines);
void show_help(void);
int print_info(int errs,int level);
int readchar(void);

#endif