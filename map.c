#include <stdlib.h>
#include "map.h"
#include "utils.h"

//check if there is enough free space for a room
bool has_space(int y, int x, int radius) {
	if (y-radius<1 || x-radius<1 || y+radius>=Y_-1 || x+radius>=X_-1)
		return false;
	for (int yy=y-radius-1; yy<=y+radius+1; yy++)
		for (int xx=x-radius-1; xx<=x+radius+1; xx++)
			if (FLOOR==map[yy][xx].type)
				return false;
	return true;
}

void dig_tile(int y, int x) {
	map[y][x].type=FLOOR;
}
void dig_path(int y0, int x0, int y1, int x1) {
	los(y0,x0,y1,x1,(char)NULL,&dig_tile);
}
bool dig_room(int y, int x, int radius, bool radial) {
	if (!has_space(y,x,radius))
		return false;

	for (int yy=y-radius; yy<=y+radius; yy++)
		for (int xx=x-radius; xx<=x+radius; xx++)
			if ((radial&&in_range(y,x,yy,xx,radius)) || !radial)
				map[yy][xx].type=FLOOR;

	return true;
}
void dig_level() {
	int new_ry=0;
	int new_rx=0;
	int radius=1+rand()%ROOM_RADIUS;
	//radial or square room
	bool radial=(bool)rand()%2;
	while (true) {
		//continue digging from the last new room or
		//dig the first room in the middle of the level
		int ry=(0==new_ry?Y_/2:new_ry);
		int rx=(0==new_rx?X_/2:new_rx);
		if ((0!=new_rx&&0!=new_ry) || dig_room(ry,rx,radius,radial)) {
			int paths=1+rand()%PATHS;
			for (int p=0;p<paths;p++) {
				int tries=0;
				//try to find an empty space and dig a room there
				while (tries++<10000 &&
				        !dig_room(new_ry=ry+rand()%(8*radius)-4*radius,
				                  new_rx=rx+rand()%(8*radius)-4*radius,
				                  radius=1+rand()%ROOM_RADIUS,
				                  radial=rand()%2)
				      );
				if (tries>10000)
					return;
				//connect the old room to the new room
				dig_path(ry,rx,new_ry,new_rx);
			}
		}
	}
}

void init_map() {
	for (int y=0; y<Y_; y++) {
		for (int x=0; x<X_; x++) {
			map[y][x].type=WALL;
			map[y][x].y=y;
			map[y][x].x=x;
			view_m[y][x]=UNSEEN;
		}
	}
	dig_level();

	//entry to next level
	int ny,nx;
	while (WALL== map[ny=rand()%Y_][nx=rand()%X_].type);
	map[ny][nx].type=NEXT_LEVEL;
}

