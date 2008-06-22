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
	mvaddstr(LINES/2-1,COLS/2-9," YOU HAVE WON! :) ");
	readchar();
	exit(end_curses());
}
void you_lost(void) {
	mvaddstr(LINES/2-1,COLS/2-9," YOU HAVE LOST! :( ");
	readchar();
	exit(end_curses());
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
	case '5':
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
	case '?':
		refresh();
		show_help();
		return false;
	default:
		return false;
	}
}

int main(void) {
	//current dungeon level
	int level=1;

	srand((unsigned)time(NULL));
	int error_lines=init_curses();

	init_map();
	init_ents(level);
	init_items();

	//the player's coordinates
	int *y=&ent_l[0].y;
	int *x=&ent_l[0].x;

	//draw start conditions
	fov(*y,*x, FOV_RADIUS);
	draw_screen();

	//message window
	int info_lines=print_info(error_lines,level);
	refresh();
	init_message_win(info_lines);

	//help window
	show_help();

	unsigned int turn=0;
	bool lost=false;
	while (!lost) {
		turn++;

		//acting on player's input
		while (!player_action(readchar(),y,x,&level));

		//use unused item if the player is standing on one
		item_t* ci=item_m[*y][*x];
		if (NULL!=ci && !ci->used) {
			if (MED_PACK==ci->type) {
				if (ent_l[0].hp<PLAYER_HP) {
					//heal hp
					ent_l[0].hp=min(ent_l[0].hp+MED_CHARGE,PLAYER_HP);
					ci->used=true;
					add_message("You feel healthy.",ci->color);
				} else
					add_message("A med pack.",0);
			} else if (AIR_CAN==ci->type) {
				if (ent_l[0].air<PLAYER_AIR) {
					//replenish air
					ent_l[0].air=min(ent_l[0].air+AIR_CHARGE,PLAYER_AIR);
					ci->used=true;
					add_message("You replenish your air supply.",ci->color);
				} else
					add_message("An air cannister.",0);
			} else if (BATTERY==ci->type) {
				if (ent_l[0].battery<PLAYER_BATTERY) {
					//charge battery
					ent_l[0].battery=min(ent_l[0].battery+BATTERY_CHARGE,PLAYER_BATTERY);
					ci->used=true;
					add_message("You charge your battery.",ci->color);
				} else
					add_message("A battery.",0);
			} else if (COIN==ci->type) {
				//take coin
				ent_l[0].coins+=COIN_CHARGE;
				ci->used=true;
				add_message("You've found a gold coin.",ci->color);
			}
		}

		//move living enemies in the player's direction
		for (int e=1;e<ENTS_;e++) {
			if (ent_l[e].hp>0 && ent_l[e].speed && turn%ent_l[e].speed)
				move_enemy(&ent_l[e],&ent_l[0]);
			if (ent_l[0].hp<1) {
				lost=true;
				break;
			}
		}

		//mark last turn's field of view as SEEN
		for (int yy=0;yy<Y_;yy++)
			for (int xx=0;xx<X_;xx++)
				if (IN_SIGHT==view_m[yy][xx])
					view_m[yy][xx]=SEEN;

		//decrease battery
		if (ent_l[0].battery>0)
			ent_l[0].battery--;

		//decrease air
		if (--ent_l[0].air<1) {
			add_message("You suffocate!",C_AIR|A_STANDOUT);
			lost=true;
		} else if (ent_l[0].air<=AIR_CHARGE &&
		           (100*ent_l[0].air/PLAYER_AIR)%5==0)
			add_message("DANGER - LOW AIR SUPPLY.",C_AIR|A_BOLD);

		//mark current field of view as IN_SIGHT
		fov(*y,*x, FOV_RADIUS);

		//draw screen
		draw_screen();
		print_info(error_lines,level);
	}
	you_lost();
}

