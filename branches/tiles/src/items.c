/*
    items.c - air cans, med packs, batteries, gold coins, etc..

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
#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "utils.h"
#include "entities.h"
#include "items.h"
#include "io.h"

#ifdef __SDL__
Mix_Chunk *money = NULL;
Mix_Chunk *clunk = NULL;
#endif

void init_items(void) {
#ifdef __SDL__
	if(!money)
		money = Mix_LoadWAV("media/money.wav");
	if(!clunk)
		clunk = Mix_LoadWAV("media/clunk.wav");
#endif
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

void use_item(ent_t *pl) {
	item_t *ci = item_m[pl->y][pl->x];
	if (NULL!=ci && !ci->used) {
		if (MED_PACK==ci->type) {
			if (pl->hp<PLAYER_HP) {
				//heal hp
				pl->hp=min(pl->hp+MED_CHARGE,PLAYER_HP);
				ci->used=true;
#ifdef __SDL__
				Mix_PlayChannel(-1, clunk, 0);
#endif
				add_message("You feel healthy.",ci->color);
			} else
				add_message("A med pack.",0);
		} else if (AIR_CAN==ci->type) {
			if (pl->air<PLAYER_AIR) {
				//replenish air
				pl->air=min(pl->air+AIR_CHARGE,PLAYER_AIR);
				ci->used=true;
#ifdef __SDL__
				Mix_PlayChannel(-1, clunk, 0);
#endif
				add_message("You replenish your air supply.",ci->color);
			} else
				add_message("An air cannister.",0);
		} else if (BATTERY==ci->type) {
			if (pl->battery<PLAYER_BATTERY) {
				//charge battery
				pl->battery=min(pl->battery+BATTERY_CHARGE,PLAYER_BATTERY);
				ci->used=true;
#ifdef __SDL__
				Mix_PlayChannel(-1, clunk, 0);
#endif
				add_message("You charge your battery.",ci->color);
			} else
				add_message("A battery.",0);
		} else if (COIN==ci->type) {
			//take coin
			pl->coins+=COIN_CHARGE;
			ci->used=true;
#ifdef __SDL__
				Mix_PlayChannel(-1, money, 0);
#endif
			add_message("You've found a gold coin.",ci->color);
		}
	}
}
