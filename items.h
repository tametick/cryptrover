#ifndef _ITEMS_H
#define _ITEMS_H

typedef struct {
	int y,x;
	chtype type;
	attr_t color;
	bool used;
}item_t;

//items
#define MED_PACK '+'
#define AIR_CAN '*'
#define BATTERY '!'
#define COIN '$'
#define MED_CHARGE 3
#define AIR_CHARGE 21
#define BATTERY_CHARGE 36
#define COIN_CHARGE 1
#define MED_NUM 3
#define AIR_NUM 4
#define BATTERY_NUM 3
#define COIN_NUM 5
#define ITEMS_ (MED_NUM+AIR_NUM+BATTERY_NUM+COIN_NUM)
extern item_t item_l[];
extern item_t *item_m[Y_][X_];

void init_items(void);

#endif
