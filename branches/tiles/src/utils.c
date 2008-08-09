/*
    utils.c - relatively general purpose utility functions

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
#include <math.h>
#include "map.h"
#include "utils.h"

void swap(int *i, int *j) {
	int t=*i;
	*i=*j;
	*j=t;
}
int min(int i,int j) {
	return i<j?i:j;
}
int max(int i,int j) {
	return i>j?i:j;
}
int dist(int y0,int x0,int y1,int x1) {
	return pow(y0-y1,2)+pow(x0-x1,2);
}
bool in_range(int y0,int x0,int y1,int x1,int r) {
	return dist(y0,x0,y1,x1)<=pow(r,2);
}

//line of sight
bool los(int y0,int x0,int y1,int x1,chtype opaque,void(*apply)(int,int)) {
	//Bresenham's line algorithm
	//taken from: http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
	bool steep=fabs(y1-y0)>fabs(x1-x0);
	if (steep) {
		swap(&x0,&y0);
		swap(&x1,&y1);
	}
	if (x0>x1) {
		swap(&x0,&x1);
		swap(&y0,&y1);
	}
	float err_num=0.0;
	int y=y0;
	for (int x=x0; x<=x1; x++) {
		if (x>x0 && x<x1) {
			if (steep) {
				if (opaque==tile_m[x][y].type)
					return false;
				else if (apply)
					apply(x,y);
			} else {
				if (opaque==tile_m[y][x].type)
					return false;
				else if (apply)
					apply(y,x);
			}
		}

		err_num+=(float)(fabs(y1-y0))/(float)(x1-x0);
		if (0.5<fabs(err_num)) {
			y+=y1>y0?1:-1;
			err_num--;
		}
	}
	return true;
}
