#ifndef INVENTORY_H
#define INVENTORY_H
#include "palette.h"
#define MAX_INVENTORY_SLOTS 20

typedef struct {
    int id;
    char name[32];
    char description[128];
    Color rarityColor; // For that "Bebop" flair
} ItemDefinition;

// The "Item Database" - The master list of everything in the world
static const ItemDefinition ITEM_DB[] = {
    { 0, "Empty Slot", "", GRAY },
    { 1, "Cracked VHS", "Labeled 'Highland High '94'. It smells like old carpet.", COLOR_DUSTY_ROSE },
    { 2, "The Tower Card", "A Tarot card found in a storm drain. It's soggy.", COLOR_TAROT_GOLD },
    { 3, "Rusty Tab", "From a soda can discontinued in the 80s.", COLOR_MUSTARD }
};

typedef enum{
  ITEM_VHS_TAPE,
  ITEM_TAROT_CARD,
  ITEM_MISC,
  ITEM_KEY_ITEM
} ItemType;



typedef struct Inventory{
  int itemIds[MAX_INVENTORY_SLOTS];
  int count;
  int selected;
} Inventory;

void InitInventory(Inventory* inventory);

#endif