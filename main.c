#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "io.h"

tile_t tile_m[Y_][X_];
view_t view_m[Y_][X_];

ent_t ent_l[ENTS_];
ent_t *ent_m[Y_][X_];

item_t item_l[ITEMS_];
item_t *item_m[Y_][X_];

void you_won(void) {
	mvaddstr(Y_/2,X_/2," YOU HAVE WON! :) ");
	readchar();
	exit(endwin());
}
void you_lost(void) {
	mvaddstr(Y_/2,X_/2," YOU HAVE LOST! :( ");
	readchar();
	exit(endwin());
}

bool player_action(int key,int *y,int *x, int *level) {
	switch (key) {
	case 'k'://up
	case '8':
		return move_to(y,x,-1,0);
	case '2'://down
	case 'j':
		return move_to(y,x,1,0);
	case 'h'://left
	case '4':
		return move_to(y,x,0,-1);
	case 'l'://right
	case '6':
		return move_to(y,x,0,1);
	case 'y'://upper left
	case '7':
		return move_to(y,x,-1,-1);
	case 'u'://upper right
	case '9':
		return move_to(y,x,-1,1);
	case 'b'://lower left
	case '1':
		return move_to(y,x,1,-1);
	case 'n'://lower right
	case '3':
		return move_to(y,x,1,1);
	case '.'://wait
	case KP_DEL:
		return true;
	case '<'://next level
	case ',':
		if (NEXT_LEVEL==tile_m[*y][*x].type) {
			if (++(*level)>LAST_LEVEL)
				you_won();
			init_map();
			init_ents(*level);
			init_items();
			return true;
		} else
			return false;
	case ESC:
	case 'q':
	case CTRL_C:
		you_lost();
	default:
		return false;
	}
}

int main(void) {
	//current dungeon level
	int level=1;

	srand((unsigned)time(NULL));
	int errs=init_curses();
	init_map();
	init_ents(level);
	init_items();

	//the player's coordinates
	int *y=&ent_l[0].y;
	int *x=&ent_l[0].x;

	while (ent_l[0].hp>0 && ent_l[0].air>0) {
		//mark last turn's field of view as SEEN
		for (int yy=0;yy<Y_;yy++)
			for (int xx=0;xx<X_;xx++)
				if (IN_SIGHT==view_m[yy][xx])
					view_m[yy][xx]=SEEN;

		//mark current field of view as IN_SIGHT
		fov(*y,*x, FOV_RADIUS);

		draw_screen();
		print_info(errs,level);

		ent_l[0].air--;

		//acting on player's input
		while (!player_action(readchar(),y,x,&level));

		//use unused item if the player is standing on one
		item_t* ci=item_m[*y][*x];
		if (NULL!=ci && !ci->used) {
			//heal hp
			if (MED_PACK==ci->type && ent_l[0].hp<PLAYER_HP) {
				ent_l[0].hp=min(ent_l[0].hp+MED_CHARGE,PLAYER_HP);
				ci->used=true;
			}
			//replenish air
			if (AIR_CAN==ci->type && ent_l[0].air<PLAYER_AIR) {
				ent_l[0].air=min(ent_l[0].air+AIR_CHARGE,PLAYER_AIR);
				ci->used=true;
			}
		}

		//move living enemies in the player's direction
		for (int e=1;e<ENTS_;e++) {
			if (ent_l[e].hp>0)
				move_enemy(&ent_l[e],&ent_l[0]);
		}
	}
	you_lost();
}

