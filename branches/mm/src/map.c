/*
    map.c - map (dungeon level) generation

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
#include <stdlib.h>
#include "map.h"
#include "utils.h"

//check if there is enough free space for a room
bool has_space(int y, int x, int radius) {
	if (y-radius<1 || x-radius<1 || y+radius>=Y_-1 || x+radius>=X_-1)
		return false;
	for (int yy=y-radius-1; yy<=y+radius+1; yy++)
		for (int xx=x-radius-1; xx<=x+radius+1; xx++)
			if (FLOOR==tile_m[yy][xx].type)
				return false;
	return true;
}

void dig_tile(int y, int x) {
	tile_m[y][x].type=FLOOR;
}
void dig_path(int y0, int x0, int y1, int x1) {
	los(y0,x0,y1,x1,(chtype)NULL,&dig_tile);
}
bool dig_room(int y, int x, int radius, bool radial) {
	if (!has_space(y,x,radius))
		return false;

	for (int yy=y-radius; yy<=y+radius; yy++)
		for (int xx=x-radius; xx<=x+radius; xx++)
			if ((radial&&in_range(y,x,yy,xx,radius)) || !radial)
				tile_m[yy][xx].type=FLOOR;

	return true;
}
void dig_level(void) {
	int new_ry=0;
	int new_rx=0;
	int radius=1+rand()%ROOM_RADIUS;
	//radial or square room
	bool radial=(bool)rand()%2;
	while (true) {
		//continue digging from the last new room or
		//dig the first room in the middle of the level
		int ry=(new_ry?new_ry:Y_/2);
		int rx=(new_rx?new_rx:X_/2);
		if ((new_rx&&new_ry) || dig_room(ry,rx,radius,radial)) {
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

void init_map(void) {
	for (int y=0; y<Y_; y++) {
		for (int x=0; x<X_; x++) {
			tile_m[y][x].type=WALL;
			tile_m[y][x].y=y;
			tile_m[y][x].x=x;
			view_m[y][x]=UNSEEN;
		}
	}
	dig_level();

	//entry to next level
	int ny,nx;
	while (WALL==tile_m[ny=rand()%Y_][nx=rand()%X_].type);
	tile_m[ny][nx].type=NEXT_LEVEL;
}
