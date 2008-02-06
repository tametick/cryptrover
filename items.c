#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "entities.h"
#include "items.h"

void init_items(void) {
	memset(item_m,(int)NULL,sizeof(item_t *)*Y_*X_);

	for (int i=0; i<ITEMS_; i++) {
		item_t *ci=&item_l[i];
		do {
			ci->y=rand()%Y_;
			ci->x=rand()%X_;
		} while (WALL==tile_m[ci->y][ci->x].type ||
				 NEXT_LEVEL==tile_m[ci->y][ci->x].type ||
		         ent_m[ci->y][ci->x] ||
		         item_m[ci->y][ci->x]);
		if (i<ITEMS_/2) {
			ci->type=MED_PACK;
			ci->color=COLOR_MED;
		} else {
			ci->type=AIR_CAN;
			ci->color=COLOR_AIR;				
		}
		ci->used=false;

		item_m[ci->y][ci->x]=ci;
	}
}
