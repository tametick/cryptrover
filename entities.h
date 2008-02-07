#ifndef _ENTITIES_H
#define _ENTITIES_H

#ifdef __WIN32__
#include "pdc28_ming_w32\curses.h"
#else
#include <ncurses.h>
#endif

typedef struct {
	int id,y,x,hp,air,speed;
	chtype type;
	attr_t color;
	bool awake;
}ent_t;

//number of entities
#define ENTS_ 12
//player attributes
#define PLAYER_HP (MED_CHARGE*5)
#define PLAYER_AIR (AIR_CHARGE*5)
#define FOV_RADIUS 5
//enemy attributes
#define CORPSE '%'
#define ARACHNID 'a'

extern ent_t ent_l[];
extern ent_t *ent_m[Y_][X_];

int compare_tiles(const void* t1, const void* t2);
void fov(int y, int x, int radius);
void init_ents(int level);
void move_enemy(ent_t *enemy, ent_t *player);
bool move_to(int *y,int *x,int dy,int dx);

#endif
