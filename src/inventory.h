#ifndef INVENTORY_H
#define INVENTORY_H
#include "palette.h"
#include "common.h"
#define MAX_INVENTORY_SLOTS 20

typedef struct Inventory{
  int itemIds[MAX_INVENTORY_SLOTS];
  int count;
  int selected;
} Inventory;

void InitInventory(Inventory* inventory);

#endif