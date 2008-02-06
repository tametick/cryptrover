#include "map.h"
#include "entities.h"
#include "items.h"
#include "io.h"
#include "mdport.h"

int init_curses(void) {
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

int readchar(void) {
	return md_readchar(stdscr);
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

void draw_screen(void) {
	//draw map
	for (int yy=0; yy<Y_; yy++) {
		for (int xx=0; xx<X_ ;xx++) {
			chtype tile=tile_m[yy][xx].type;
			if (IN_SIGHT==view_m[yy][xx]) {
				if (WALL==tile)
					//to distinguish it from SEEN walls
					mvaddch(yy,xx,tile|A_BOLD);
				else
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
}

