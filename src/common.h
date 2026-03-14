#ifndef COMMON_H
#define COMMON_H

typedef enum{
  ITEM_VHS_TAPE,
  ITEM_TAROT_CARD,
  ITEM_MISC,
  ITEM_KEY_ITEM,
  ITEM_MINERAL
} ItemType;
typedef struct {
    int id;
    char name[32];
    char description[128];
    // Co lor rarityColor;
    ItemType type; // For that "Bebop" flair
} ItemDefinition;

typedef struct Dialog
{
  char character_name[32];
  char text[256];
  int id;
  int nextid;
} Dialog;
#endif