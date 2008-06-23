#include "map.h"
#include "entities.h"
#include "items.h"
#include "io.h"
#include "mdport.h"

WINDOW *message_win;
int message_win_height;
int messages;

int init_curses(void) {
	int error_lines=0;
	stdscr=initscr();
	if (ERR==keypad(stdscr,true))
		mvaddstr(error_lines++,X_+1,"Cannot enable keypad");
	if (ERR==noecho())
		mvaddstr(error_lines++,X_+1,"Cannot set noecho mode");
	if (ERR==start_color() || !has_colors())
		mvaddstr(error_lines++,X_+1,"Cannot enable colors");
	else {
		for (int c=0; c<=8; c++)
			init_pair(c,c,0);
	}
	if (ERR==curs_set(0))
		mvaddstr(error_lines++,X_+1,"Cannot set invisible cursor");

	if(error_lines){
		attron(A_STANDOUT);
		mvaddstr(error_lines++,X_+1,"Please try a different terminal");
		attroff(A_STANDOUT);
	}
	return error_lines;
}

int end_curses(void) {
	delwin(message_win);
	delwin(stdscr);
	return endwin();
}

void show_help(void) {
	WINDOW *help_win=newwin(Y_*3/4,X_*3/4,Y_/8,X_/8);
	PANEL *help_panel=new_panel(help_win);
	box(help_win,0,0);
	int lines=1;
	mvwaddstr(help_win,lines++,1,"To move or attack, use the numpad");
	mvwaddstr(help_win,lines++,1,"or vi keys:");
	mvwaddstr(help_win,lines++,1,"\t7 8 9    y k u");
	mvwaddstr(help_win,lines++,1,"\t \\|/      \\|/ ");
	mvwaddstr(help_win,lines++,1,"\t4-5-6    h-.-l");
	mvwaddstr(help_win,lines++,1,"\t /|\\      /|\\ ");
	mvwaddstr(help_win,lines++,1,"\t1 2 3    b j n");
	lines++;
	mvwaddstr(help_win,lines++,1,"To go up a staircase to the next ");
	mvwaddstr(help_win,lines++,1,"level press '<' or ','.");
	lines++;
	mvwaddstr(help_win,lines++,1,"To quit the game press 'q', ESC ");
	mvwaddstr(help_win,lines++,1,"or Ctrl+c.");
	lines++;
	mvwaddstr(help_win,lines++,1,"To come back to this help screen ");
	mvwaddstr(help_win,lines++,1,"press '?'.");
	wrefresh(help_win);
	show_panel(help_panel);
	update_panels();
	readchar();
	hide_panel(help_panel);
	delwin(help_win);
	del_panel(help_panel);
	refresh();
}

void init_message_win(int info_lines) {
	message_win=newwin(message_win_height=LINES-info_lines-2,COLS-X_-1,info_lines+2,X_+1);
	scrollok(message_win,true);
	wrefresh(message_win);
	messages=0;
}

void add_message(char *msg, attr_t attr) {
	wattron(message_win,attr);
	if (messages>=message_win_height-1) {
		mvwaddstr(message_win,messages,0,msg);
		scroll(message_win);
	} else
		mvwaddstr(message_win,messages++,0,msg);

	wrefresh(message_win);
	wattroff(message_win,attr);
}

int readchar(void) {
	return md_readchar(stdscr);
}

int print_info(int errs,int level) {
	int msgs=errs;
	char msg[COLS-X_-1];
	sprintf(msg, "Hit points: %d%%     ",100*ent_l[0].hp/PLAYER_HP);
	mvaddstr(msgs++,X_+1,msg);
	sprintf(msg, "Air: %d%%            ",100*ent_l[0].air/PLAYER_AIR);
	mvaddstr(msgs++,X_+1,msg);
	sprintf(msg, "Battery: %d%%        ",100*ent_l[0].battery/PLAYER_BATTERY);
	mvaddstr(msgs++,X_+1,msg);
	if(1==ent_l[0].coins)
		sprintf(msg, "Gold: %d coin        ",ent_l[0].coins);
	else
		sprintf(msg, "Gold: %d coins       ",ent_l[0].coins);
	mvaddstr(msgs++,X_+1,msg);
	sprintf(msg, "Dungeon level: %d/%d ",level,LAST_LEVEL);
	mvaddstr(msgs++,X_+1,msg);
	mvaddstr(++msgs,X_+1,"Items:" );
	mvaddch(++msgs,X_+1,MED_PACK|C_MED);
	addstr(" - med pack ");
	mvaddch(++msgs,X_+1,AIR_CAN|C_AIR);
	addstr(" - air cannister ");
	mvaddch(++msgs,X_+1,BATTERY|C_BAT);
	addstr(" - battery ");
	mvaddch(++msgs,X_+1,COIN|C_COIN);
	addstr(" - gold coin ");
	return msgs+1;
}

void draw_screen(void) {
	//draw map
	for (int yy=0; yy<Y_; yy++) {
		for (int xx=0; xx<X_ ;xx++) {
			chtype tile=tile_m[yy][xx].type;
			if (IN_SIGHT==view_m[yy][xx]) {
				mvaddch(yy,xx,tile);
			} else if (SEEN==view_m[yy][xx]) {
				mvaddch(yy,xx,tile|C_FOG);
			} else {
				mvaddch(yy,xx,' ');
			}
		}
	}
	//draw corpses
	for (int e=0; e<ENTS_; e++) {
		if ((ent_l[e].hp<1 || ent_l[e].air<1) &&
		        view_m[ent_l[e].y][ent_l[e].x]==IN_SIGHT &&
		        NEXT_LEVEL!=tile_m[ent_l[e].y][ent_l[e].x].type)
			mvaddch(ent_l[e].y,ent_l[e].x,CORPSE|ent_l[e].color);
	}
	//draw items
	for (int i=0; i<ITEMS_; i++) {
		if (!item_l[i].used) {
			if (view_m[item_l[i].y][item_l[i].x]==IN_SIGHT)
				mvaddch(item_l[i].y,item_l[i].x,item_l[i].type|item_l[i].color);
			else if (view_m[item_l[i].y][item_l[i].x]==SEEN)
				mvaddch(item_l[i].y,item_l[i].x,item_l[i].type|C_FOG);
		}
	}
	//draw living entities
	for (int e=0; e<ENTS_; e++) {
		if (ent_l[e].hp>0 && ent_l[e].air>0 && view_m[ent_l[e].y][ent_l[e].x]==IN_SIGHT)
			mvaddch(ent_l[e].y,ent_l[e].x,ent_l[e].type|ent_l[e].color);
	}
}

