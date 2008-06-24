#ifndef _UTILS_H
#define _UTILS_H

#include <stdbool.h>

int dist(int y0,int x0,int y1,int x1);
bool in_range(int y0,int x0,int y1,int x1,int r);
bool los(int y0,int x0,int y1,int x1,chtype opaque,void(*apply)(int,int));
int max(int i,int j);
int min(int i,int j);
void swap(int *i, int *j);

#endif
