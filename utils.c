#include <math.h>
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
