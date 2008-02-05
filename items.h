#ifndef _ITEMS_H
#define _ITEMS_H

//items
#define ITEMS_ 6
#define MED_PACK ('+'|COLOR_PAIR(COLOR_GREEN))
#define AIR_CAN ('*'|COLOR_PAIR(COLOR_BLUE))
#define MED_CHARGE 5
#define AIR_CHARGE 20
extern item item_l[];
extern item *item_m[Y_][X_];

void init_items(void);

#endif
