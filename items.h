#ifndef _ITEMS_H
#define _ITEMS_H

typedef struct {
	int y,x;
	chtype type;
	attr_t color;
	bool used;
}item_t;

//items
#define ITEMS_ 6
#define MED_PACK '+'
#define AIR_CAN '*'
#define COLOR_MED COLOR_PAIR(COLOR_GREEN)
#define COLOR_AIR COLOR_PAIR(COLOR_BLUE)
#define MED_CHARGE 5
#define AIR_CHARGE 20
extern item_t item_l[];
extern item_t *item_m[Y_][X_];

void init_items(void);

#endif
