#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "io.h"

void init_ents(int level) {
	memset(ent_m,(int)NULL,sizeof(ent_t *)*Y_*X_);
	for (int e=0; e<ENTS_; e++) {
		ent_t *ce=&ent_l[e];
		ce->id=e;
		ce->awake=false;
		do {
			ce->y=rand()%Y_;
			ce->x=rand()%X_;
		} while (WALL==tile_m[ce->y][ce->x].type || NULL!=ent_m[ce->y][ce->x]);
		if (e>0) {
			ce->hp=2;
			ce->air=1;
			ce->speed=3;
			ce->battery=1;
			ce->type=ARACHNID;
			ce->color=COLOR_PAIR(COLOR_RED);
		}
		ent_m[ce->y][ce->x]=ce;
	}
	//initial player attributes
	if (1==level) {
		ent_l[0].hp=PLAYER_HP;
		ent_l[0].air=PLAYER_AIR;
		ent_l[0].speed=0;//special case: move every turn
		ent_l[0].battery=PLAYER_BATTERY;
		ent_l[0].type='@';
		ent_l[0].color=COLOR_PAIR(COLOR_WHITE);
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

//radial field of view
void fov(int y, int x, int radius) {
	for (int yy=max(y-radius,0); yy<=min(y+radius,Y_-1); yy++)
		for (int xx=max(x-radius,0); xx<=min(x+radius,X_-1); xx++)
			if (in_range(y,x,yy,xx,radius*ent_l[0].battery/PLAYER_BATTERY)
			        && los(y,x,yy,xx,WALL,NULL))
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
				add_message("The arachnid bites you.",0);
				if (de->hp<=MED_CHARGE && de->hp>0)
					add_message("DANGER - LOW HITPOINTS.",C_MED|A_BOLD);
			} else
				add_message("You hit the arachnid.",0);
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
	        (in_range(*ey,*ex,player->y,player->x,FOV_RADIUS) &&
	         los(*ey,*ex,player->y,player->x,WALL,NULL))) {
		if (!enemy->awake) {
			enemy->awake=true;
			add_message("The arachnid notices you.",0);
		}

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
