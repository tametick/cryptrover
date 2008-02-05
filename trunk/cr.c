#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "io.h"

tile map[Y_][X_];
view view_m[Y_][X_];

ent ent_l[ENTS_];
ent *ent_m[Y_][X_];

item item_l[ITEMS_];
item *item_m[Y_][X_];

int end_game() {
	readchar();
	exit(endwin());
}
void you_won() {
	mvaddstr(Y_/2,X_/2," YOU HAVE WON! :) ");
	end_game();
}
void you_lost() {
	mvaddstr(Y_/2,X_/2," YOU HAVE LOST! :( ");
	end_game();
}

int main() {
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


		//move living enemies in the player's direction
		for (int e=1;e<ENTS_;e++) {
			if (ent_l[e].hp>0)
				move_enemy(&ent_l[e],&ent_l[0]);
		}

		//use unused item if the player is standing on one
		item* ci=item_m[*y][*x];
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
		
		status st=handle_input(readchar(),y,x,level);
		if (LOST==st)
			you_lost();
		else if (WON==st)
			you_won();

	}
	you_lost();
}

