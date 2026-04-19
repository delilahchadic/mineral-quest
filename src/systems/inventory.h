#ifndef INVENTORY_H
#define INVENTORY_H
#include "engine/palette.h"
#include "engine/common.h"
#define MAX_INVENTORY_SLOTS 20

typedef struct Inventory{
  int itemIds[MAX_INVENTORY_SLOTS];
  int count;
} Inventory;

void InitInventory(Inventory* inventory);

#endif