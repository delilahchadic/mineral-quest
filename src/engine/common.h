#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include "raylib.h"


typedef enum ItemType{
  ITEM_VHS_TAPE,
  ITEM_TAROT_CARD,
  ITEM_MISC,
  ITEM_KEY_ITEM,
  ITEM_MINERAL
} ItemType;

typedef struct ItemDefinition{
    int id;
    char name[32];
    char description[128];
    // Co lor rarityColor;
    ItemType type; // For that "Bebop" flair
} ItemDefinition;

typedef struct Dialog {
  char character_name[32];
  char text[256];
  int id;
  int nextid;
} Dialog;

typedef struct Plant{
  char species_name[32];
  Texture2D sprite;
  int frameheight;
  int framewidth;
  int hitboxheight;
  int hitboxwidth;
  uint32_t default_trait_flags;
} Plant;

typedef enum { TILE_WATER,TILE_GRASS ,TILE_SAND, TILE_DIRT,TILE_STONE, TILE_ROAD} TileType;


typedef struct TileDefinition{
    int id;             // 0 = Sand, 1 = Asphalt, 2 = Magnetic Pit
    bool is_blocking;   //
    float friction;     // 1.0 = Normal, 0.2 = Ice/Oil, 1.5 = Deep Sand
    int footstep_sfx;
    Color color;// sound??
} TileDefinition;

typedef enum EntityType{
    ENTITY_CHARACTER,
    ENTITY_ITEM,
    ENTITY_PLANT,
    ENTITY_DECOR,
    ENTITY_PLAYER
} EntityType;

typedef enum TraitFlags{
  TRAIT_NONE = 0,
  TRAIT_TALK = 1 << 0
} TraitFlags;

typedef struct Character{
  /* data */
  char name[32];
  Texture2D sprite;
  int dialogId;
  uint32_t default_trait_flags;
} Character;
#endif