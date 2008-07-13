/*
    io.c - handling keyboard input and screen output

    Copyright 2008 Ido Yehieli

    This file is part of CryptRover.

    CryptRover is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CryptRover is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CryptRover.  If not, see <http://www.gnu.org/licenses/>.
 */
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

	if (error_lines) {
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
	mvwaddstr(help_win,lines++,1,"To move or attack use wasd,");
	mvwaddstr(help_win,lines++,1,"vi keys or the numpad:");
	mvwaddstr(help_win,lines++,1,"     q w e    y k u    7 8 9");
	mvwaddstr(help_win,lines++,1,"      \\|/      \\|/      \\|/ ");
	mvwaddstr(help_win,lines++,1,"     a-s-d    h-.-l    4-5-6");
	mvwaddstr(help_win,lines++,1,"      /|\\      /|\\      /|\\ ");
	mvwaddstr(help_win,lines++,1,"     z x c    b j n    1 2 3");
	lines++;
	mvwaddstr(help_win,lines++,1,"To go up a staircase to the next ");
	mvwaddstr(help_win,lines++,1,"level press '<' or ','.");
	lines++;
	mvwaddstr(help_win,lines++,1,"To flip the flashlight on and off");
	mvwaddstr(help_win,lines++,1,"press 'f'.");
	lines++;
	mvwaddstr(help_win,lines++,1,"To quit the game press ESC or");
	mvwaddstr(help_win,lines++,1,"Ctrl+c.");
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
	mvaddstr(msgs,X_+1,"[----------]");
	move(msgs++,X_+2);
	for(int i=0;i<10*ent_l[0].hp/PLAYER_HP;i++)
		addch('*'|C_MED);
	sprintf(msg, "Air: %d%%            ",100*ent_l[0].air/PLAYER_AIR);
	mvaddstr(msgs++,X_+1,msg);
	mvaddstr(msgs,X_+1,"[----------]");
	move(msgs++,X_+2);
	for(int i=0;i<10*ent_l[0].air/PLAYER_AIR;i++)
		addch('*'|C_AIR);
	sprintf(msg, "Battery: %d%%        ",100*ent_l[0].battery/PLAYER_BATTERY);
	mvaddstr(msgs++,X_+1,msg);
	mvaddstr(msgs,X_+1,"[----------]");
	move(msgs++,X_+2);
	for(int i=0;i<10*ent_l[0].battery/PLAYER_BATTERY;i++)
		addch('*'|C_BAT);
	if (1==ent_l[0].coins)
		sprintf(msg, "Gold: %d coin        ",ent_l[0].coins);
	else
		sprintf(msg, "Gold: %d coins       ",ent_l[0].coins);
	mvaddstr(msgs++,X_+1,msg);
	sprintf(msg, "Dungeon level: %d/%d ",level,LAST_LEVEL);
	mvaddstr(msgs++,X_+1,msg);
	mvaddstr(msgs,X_+1,"[------------]");
	move(msgs++,X_+2);
		for(int i=0;i<level;i++)
			addch('x'|C_LVL);

	mvaddstr(++msgs,X_+1,"Items:" );
	mvaddch(++msgs,X_+1,MED_PACK|C_MED);
	addstr(" - med pack ");
	mvaddch(++msgs,X_+1,AIR_CAN|C_AIR);
	addstr(" - air cannister ");
	mvaddch(++msgs,X_+1,BATTERY|C_BAT);
	addstr(" - battery ");
	mvaddch(++msgs,X_+1,COIN|C_COIN);
	addstr(" - gold coin ");
	return msgs;
}

void draw_screen(void) {
	//draw map
	for (int yy=0; yy<Y_; yy++) {
		for (int xx=0; xx<X_ ;xx++) {
			chtype tile=tile_m[yy][xx].type;
			if (IN_SIGHT==view_m[yy][xx]) {
				mvaddch(yy,xx,tile|tile_color_m[yy][xx]);
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
				NEXT_LEVEL!=tile_m[ent_l[e].y][ent_l[e].x].type){
			if (view_m[ent_l[e].y][ent_l[e].x]==IN_SIGHT)
				mvaddch(ent_l[e].y,ent_l[e].x,CORPSE|ent_l[e].color);
			else if (view_m[ent_l[e].y][ent_l[e].x]==SEEN)
				mvaddch(ent_l[e].y,ent_l[e].x,CORPSE|C_FOG);
		}
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

