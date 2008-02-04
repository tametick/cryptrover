#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "mdport.h"

//key codes
#define ESC 27
#define CTRL_C 3
#ifdef __WIN32__
#define KP_DEL 78
#else
#define KP_DEL 74
#endif

tile map[Y_][X_];
view view_m[Y_][X_];

ent ent_l[ENTS_];
ent *ent_m[Y_][X_];

item item_l[ITEMS_];
item *item_m[Y_][X_];

int init_curses() {
	int errs=0;
	stdscr=initscr();
	if (ERR==keypad(stdscr,true))
		mvaddstr(errs++,X_+1,"Cannot enable keypad");
	if (ERR==noecho())
		mvaddstr(errs++,X_+1,"Cannot set noecho mode");
	if (ERR==curs_set(0))
		mvaddstr(errs++,X_+1,"Cannot set invisible cursor");
	if (ERR==start_color() || !has_colors())
		mvaddstr(errs++,X_+1,"Cannot enable colors");
	else {
		for (int c=0; c<8; c++)
			init_pair(c,c,0);
	}
	return errs;
}

int end_game() {
	md_readchar(stdscr);
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

void handle_input(int *key,int *y,int *x, int level) {
	bool success=true;
	switch (*key) {
	case 'k'://up
	case '8':
		success=move_to(y,x,-1,0);
		break;
	case '2'://down
	case 'j':
		success=move_to(y,x,1,0);
		break;
	case 'h'://left
	case '4':
		success=move_to(y,x,0,-1);
		break;
	case 'l'://right
	case '6':
		success=move_to(y,x,0,1);
		break;
	case 'y'://upper left
	case '7':
		success=move_to(y,x,-1,-1);
		break;
	case 'u'://upper right
	case '9':
		success=move_to(y,x,-1,1);
		break;
	case 'b'://lower left
	case '1':
		success=move_to(y,x,1,-1);
		break;
	case 'n'://lower right
	case '3':
		success=move_to(y,x,1,1);
		break;
	case '.'://wait
	case KP_DEL:
		break;
	case '<'://next level
	case ',':
		if (NEXT_LEVEL==map[*y][*x].type) {
			if (++level>LAST_LEVEL)
				you_won();
			init_map();
			init_ents(level);
			init_items();
		} else {
			success=false;
		}
		break;
	case ESC:
	case 'q':
	case CTRL_C:
		you_lost();
		break;
	default:
		success=false;
		break;
	}
	if (!success) {
		*key=md_readchar(stdscr);
		handle_input(key,y,x,level);
	}
}

void print_info(int errs,int level) {
	int msgs=errs;
	char msg[50];
	sprintf(msg, "Hit points: %d%%     ",100*ent_l[0].hp/PLAYER_HP);
	mvaddstr(msgs++,X_+1,msg);
	sprintf(msg, "Air: %d%%            ",100*ent_l[0].air/PLAYER_AIR);
	mvaddstr(msgs++,X_+1,msg);
	sprintf(msg, "Dungeon level: %d/%d ",level,LAST_LEVEL);
	mvaddstr(msgs++,X_+1,msg);
	mvaddstr(++msgs,X_+1,"Items:" );
	mvaddch(++msgs,X_+1,MED_PACK);
	addstr(" - med pack ");
	mvaddch(++msgs,X_+1,AIR_CAN);
	addstr(" - air canister ");
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

	//last key pressed
	int key='.';//wait
	do {
		handle_input(&key,y,x,level);
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

		//draw map
		for (int yy=0; yy<Y_; yy++) {
			for (int xx=0; xx<X_ ;xx++) {
				chtype tile=map[yy][xx].type;
				if (IN_SIGHT==view_m[yy][xx]) {
					mvaddch(yy,xx,tile);
				} else if (SEEN==view_m[yy][xx]) {
					if (WALL==tile)
						mvaddch(yy,xx,tile);
					else
						mvaddch(yy,xx,' ');
				} else {
					mvaddch(yy,xx,' ');
				}
			}
		}
		//draw items
		for (int i=0; i<ITEMS_; i++) {
			if (!item_l[i].used && view_m[item_l[i].y][item_l[i].x]==IN_SIGHT)
				mvaddch(item_l[i].y,item_l[i].x,item_l[i].type);
		}
		//draw entities
		for (int e=0; e<ENTS_; e++) {
			if (ent_l[e].hp>0 && view_m[ent_l[e].y][ent_l[e].x]==IN_SIGHT)
				mvaddch(ent_l[e].y,ent_l[e].x,ent_l[e].type);
		}

		print_info(errs,level);
		ent_l[0].air--;
		key=md_readchar(stdscr);
	} //exit when the player is dead
	while (ent_l[0].hp>0 && ent_l[0].air>0);
	you_lost();
}

