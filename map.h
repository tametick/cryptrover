#ifndef _MAP_H
#define _MAP_H

#ifdef __WIN32__
#include "pdc28_ming_w32\curses.h"
#else
#include <ncurses.h>
#endif

typedef struct {
	int y,x;
	chtype type;
	bool used;
}item;
typedef struct {
	int y,x;
	chtype type;
}tile;

typedef enum {
	UNSEEN,
	SEEN,
	IN_SIGHT
}view;

//map attributes
#define Y_ 24
#define X_ 48
#define WALL '#'
#define FLOOR '.'
#define NEXT_LEVEL '<'


#endif
