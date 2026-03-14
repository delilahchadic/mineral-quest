#include "systems/inventory.h"

void InitInventory(Inventory* inventory){
  inventory->count =0;
  inventory->selected = 0;
  for(int i = 0; i < 20; i++) {
    inventory->itemIds[i] = 0;
  }
}