/*  Copyright 2008 Ido Yehieli

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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "io.h"

tile_t tile_m[Y_][X_];
int tile_color_m[Y_][X_];
view_t view_m[Y_][X_];

ent_t ent_l[ENTS_];
ent_t *ent_m[Y_][X_];

item_t item_l[ITEMS_];
item_t *item_m[Y_][X_];

void end_game(){
#ifdef __SDL__
	Mix_CloseAudio();
	SDL_Quit();
#endif
	exit(end_curses());
}
void you_won(void) {
	mvaddstr(LINES/2-1,COLS/2-9," YOU HAVE WON! :) ");
	readchar();
	end_game();
}
void you_lost(void) {
#ifdef __SDL__
	Mix_PlayChannel(-1, Mix_LoadWAV("media/grunt.wav"), 0);
#endif
	mvaddstr(LINES/2-1,COLS/2-9," YOU HAVE LOST! :( ");
	readchar();
	end_game();
}

bool player_action(int key,int *y,int *x, int *level) {
	switch (key) {
	case 'k'://up
	case 'w':
	case '8':
		return move_to(y,x,-1,0);
	case 'j'://down
	case 'x':
	case '2':
		return move_to(y,x,1,0);
	case 'h'://left
	case 'a':
	case '4':
		return move_to(y,x,0,-1);
	case 'l'://right
	case 'd':
	case '6':
		return move_to(y,x,0,1);
	case 'y'://upper left
	case 'q':
	case '7':
		return move_to(y,x,-1,-1);
	case 'u'://upper right
	case 'e':
	case '9':
		return move_to(y,x,-1,1);
	case 'b'://lower left
	case 'z':
	case '1':
		return move_to(y,x,1,-1);
	case 'n'://lower right
	case 'c':
	case '3':
		return move_to(y,x,1,1);
	case '.'://wait
	case 's':
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

int main(int argc, char *argv[]) {
	//initialization
	srand((unsigned)time(NULL));
	int error_lines=init_curses();
#ifdef __SDL__
	SDL_Init(SDL_INIT_AUDIO);

	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16;
	int audio_channels = 2;
	int audio_buffers = 4096;
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		mvaddstr(error_lines++,X_+1,"Unable to open audio");
	}

	Mix_Music *music = Mix_LoadMUS("media/A_Nightmare_On_Elm_Street.ogg");
	Mix_PlayMusic(music, -1);

	Mix_Chunk *alert = Mix_LoadWAV("media/alert.wav");
#endif

	//current dungeon level
	int level=1;

	init_map();
	init_ents(level);
	init_items();

	//the player and their coordinates
	ent_t *pl=&ent_l[0];
	int *y=&(pl->y);
	int *x=&(pl->x);

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
		use_item(pl);

		//move living enemies in the player's direction
		for (int e=1;e<ENTS_;e++) {
			if (ent_l[e].hp>0 && ent_l[e].speed && turn%ent_l[e].speed)
				move_enemy(&ent_l[e],pl);
			if (pl->hp<1) {
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
		if (pl->battery>0)
			pl->battery--;

		//decrease air
		if (--pl->air<1) {
			add_message("You suffocate!",C_AIR|A_STANDOUT);
			lost=true;
		} else if (pl->air<=AIR_CHARGE &&
		           (100*pl->air/PLAYER_AIR)%5==0) {
#ifdef __SDL__
			Mix_PlayChannel(-1, alert, 0);
#endif
			add_message("DANGER - LOW AIR SUPPLY.",C_AIR|A_BOLD);
		}

		//mark current field of view as IN_SIGHT
		fov(*y,*x, FOV_RADIUS);

		//draw screen
		draw_screen();
		print_info(error_lines,level);
	}
	you_lost();
}

