#ifndef _ITEMS_H
#define _ITEMS_H

typedef struct {
	int y,x;
	chtype type;
	attr_t color;
	bool used;
}item_t;

//items
#define ITEMS_ 9
#define MED_PACK '+'
#define AIR_CAN '*'
#define BATTERY '!'
#define MED_CHARGE 3
#define AIR_CHARGE 21
#define BATTERY_CHARGE 40
extern item_t item_l[];
extern item_t *item_m[Y_][X_];

void init_items(void);

#endif
