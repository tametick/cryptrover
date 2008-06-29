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
#include "entities.h"
#include "items.h"
#include "io.h"

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
		if (i<MED_NUM) {
			ci->type=MED_PACK;
			ci->color=C_MED;
		} else if (i-MED_NUM<AIR_NUM) {
			ci->type=AIR_CAN;
			ci->color=C_AIR;
		} else if (i-MED_NUM-AIR_NUM<COIN_NUM) {
			ci->type=BATTERY;
			ci->color=C_BAT;
		} else {
			ci->type=COIN;
			ci->color=C_COIN;
		}
		ci->used=false;

		item_m[ci->y][ci->x]=ci;
	}
}
