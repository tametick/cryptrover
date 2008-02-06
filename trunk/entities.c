#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"

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
			ce->type='a';
			ce->color=COLOR_PAIR(COLOR_RED);
		}
		ent_m[ce->y][ce->x]=ce;
	}
	//initial player attributes
	if (1==level) {
		ent_l[0].hp=PLAYER_HP;
		ent_l[0].air=PLAYER_AIR;
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
			if (in_range(y,x,yy,xx,radius) && los(y,x,yy,xx,WALL,NULL))
				view_m[yy][xx]=IN_SIGHT;
}

//move entity if there is no living entity on the way
bool move_to(int *y,int *x,int dy,int dx) {
	//don't move into walls
	if (WALL==tile_m[*y+dy][*x+dx].type)
		return false;

	int id=ent_m[*y][*x]->id;
	//if the destination tile has an entity in it
	if (NULL!=ent_m[*y+dy][*x+dx]) {
		ent_t *de=ent_m[*y+dy][*x+dx];
		//to prevent enemies from attacking one another
		if (0==id||0==de->id) {
			de->hp--;
		} else {
			return false;
		}
		//if it's dead remove its reference
		if (de->hp<1) {
			ent_m[de->y][de->x]=NULL;
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
