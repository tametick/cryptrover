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
#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "io.h"

#ifdef __SDL__
Mix_Chunk *bite = NULL;
Mix_Chunk *punch = NULL;
Mix_Chunk *grunt = NULL;
Mix_Chunk *alert = NULL;
#endif

bool close_to_player(int y,int x) {
	return in_range(ent_l[0].y,ent_l[0].x,y,x,FOV_RADIUS-1);
}
void init_ents(int level) {
#ifdef __SDL__
	if(!bite)
		bite = Mix_LoadWAV("media/bite.wav");
	if(!punch)
		punch = Mix_LoadWAV("media/punch.wav");
	if(!grunt)
		grunt = Mix_LoadWAV("media/grunt.wav");
	if(!alert)
		alert = Mix_LoadWAV("media/alert.wav");
#endif
	memset(ent_m,(int)NULL,sizeof(ent_t *)*Y_*X_);
	for (int e=0; e<ENTS_; e++) {
		ent_t *ce=&ent_l[e];
		ce->id=e;
		ce->awake=false;
		do {
			ce->y=rand()%Y_;
			ce->x=rand()%X_;
		} while (WALL==tile_m[ce->y][ce->x].type
		         || NULL!=ent_m[ce->y][ce->x]
		         || (e && close_to_player(ce->y,ce->x)) );
		if (e>0) {
			ce->hp=2;
			ce->air=1;
			ce->speed=3;
			ce->battery=1;
			ce->coins=0;
			ce->type=ARACHNID;
			ce->color=COLOR_PAIR(COLOR_RED);
		}
		ent_m[ce->y][ce->x]=ce;
	}
	//initial player attributes
	ent_t *pl=&ent_l[0];
	if (1==level) {
		pl->hp=PLAYER_HP;
		pl->air=PLAYER_AIR;
		pl->speed=0;//special case: move every turn
		pl->battery=PLAYER_BATTERY;
		pl->coins=0;
		pl->type='@';
		pl->color=COLOR_PAIR(COLOR_WHITE);
	}
}

//compare 2 tiles by their distance to the player
int compare_tiles(const void* t1, const void* t2) {
	int py=ent_l[0].y;
	int px=ent_l[0].x;
	tile_t* tile1 = (tile_t*)t1;
	tile_t* tile2 = (tile_t*)t2;
	if (dist(tile1->y,tile1->x,py,px)<dist(tile2->y,tile2->x,py,px))
		return -1;
	else if (dist(tile1->y,tile1->x,py,px)==dist(tile2->y,tile2->x,py,px))
		return 0;
	else
		return 1;
}

//field of view
void fov(int y, int x, int radius) {
	int nr=radius*ent_l[0].battery/PLAYER_BATTERY;
	for (int yy=max(y-nr,0); yy<=min(y+nr,Y_-1); yy++)
		for (int xx=max(x-nr,0); xx<=min(x+nr,X_-1); xx++)
			if (los(y,x,yy,xx,WALL,NULL))
				view_m[yy][xx]=IN_SIGHT;
}

//move entity if there is no living entity on the way
bool move_to(int *y,int *x,int dy,int dx) {
	int id=ent_m[*y][*x]->id;

	//don't move into walls
	if (WALL==tile_m[*y+dy][*x+dx].type) {
		if (!id)
			add_message("There is a wall in the way!",0);
		return false;
	}

	//if the destination tile has an entity in it
	if (NULL!=ent_m[*y+dy][*x+dx]) {
		ent_t *de=ent_m[*y+dy][*x+dx];
		//to prevent enemies from attacking one another
		if (!id||!de->id) {
			de->hp--;
			if (id) {
#ifdef __SDL__
				Mix_PlayChannel(-1, bite, 0);
				Mix_PlayChannel(-1, grunt, 0);
#endif
				add_message("The arachnid bites you.",0);
				if (de->hp<=MED_CHARGE && de->hp>0){
#ifdef __SDL__
					Mix_PlayChannel(-1, alert, 0);
#endif
					add_message("DANGER - LOW HITPOINTS.",C_MED|A_BOLD);
				}
			} else {
				add_message("You hit the arachnid.",0);
#ifdef __SDL__
				Mix_PlayChannel(-1, punch, 0);
#endif
			}
		} else
			return false;

		//if it's dead remove its reference from the entity map
		if (de->hp<1) {
			ent_m[de->y][de->x]=NULL;
			if (!id)
				add_message("You kill the arachnid!",de->color);
			else
				add_message("The arachnid kills you!",C_MED|A_STANDOUT);
		}

		//the move was still successful because of the attack
		return true;
	}
	//remove reference to the entity's old position
	ent_m[*y][*x]=NULL;
	//update entity's position
	*y+=dy;
	*x+=dx;
	//add reference to the entity's new position
	ent_m[*y][*x]=&ent_l[id];
	return true;
}

void move_enemy(ent_t *enemy, ent_t *player) {
	int *ey=&enemy->y;
	int *ex=&enemy->x;
	if (enemy->awake ||
	        (abs(*ey-player->y)<=FOV_RADIUS &&
	         abs(*ex-player->x)<=FOV_RADIUS &&
	         los(*ey,*ex,player->y,player->x,WALL,NULL))) {
		if (!enemy->awake)
			enemy->awake=true;

		//sort the adjunct tiles by their distance to the player
		tile_t adj_tile[9];
		int t=0;
		for (int y=*ey-1;y<=*ey+1;y++)
			for (int x=*ex-1;x<=*ex+1;x++)
				adj_tile[t++]=tile_m[y][x];
		qsort(adj_tile,9,sizeof(tile_t),compare_tiles);

		//move to the closest possible tile
		t=0;
		while (t<9 && !move_to(ey,ex,adj_tile[t].y-*ey,adj_tile[t].x-*ex)) {
			t++;
		}
	} else {
		//sleeping enemies move randomly
		move_to(ey,ex,-1+rand()%3,-1+rand()%3);
	}
}
