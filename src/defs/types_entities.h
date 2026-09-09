#ifndef TYPES_ENTITIES
#define TYPES_ENTITIES

#include "stdint.h"
#include "raylib.h"
#include "defs/constants.h"
#include "defs/types_minerals.h"

typedef enum {
    STAT_STR,
    STAT_DEF,
    STAT_MAG_OFF,
    STAT_MAG_DEF,
    STAT_SPEED,
    STAT_GEOLOGY,
    STAT_BOTANY,
    STAT_ALCHEMY,
    STAT_AEROBICS,
    STAT_ACCESORY_COUNT,
    STAT_COUNT
} StatType;

typedef struct StatBlock {
    int base[STAT_COUNT];
    int current[STAT_COUNT];
    int max_hp;
    int current_hp;
} StatBlock;

typedef enum ItemType{
  ITEM_VHS_TAPE,
  ITEM_TAROT_CARD,
  ITEM_MISC,
  ITEM_KEY_ITEM,
  ITEM_MINERAL,
  ITEM_WEAPON,
  ITEM_ACCESSORY
} ItemType;

typedef enum {
    SLOT_NONE = -1,     // Non-equipables (Consumables, Minerals, Key Items)
    SLOT_WEAPON = 0,    // Primary weapon
    SLOT_ACCESSORY,     // Accessories (capped by player stats)
    SLOT_COUNT          // Total valid equipment slot types (2)
} EquipSlot;

typedef struct ItemDefinition {
    int id;
    char name[32];
    char description[128];
    int type;
    EquipSlot slot;                   // Explicit enum type instead of raw int
    int stat_bonuses[STAT_COUNT];     // Matches all 9 stats (STR through AEROBICS)
    int granted_ability_id;           // -1 if no ability attached
} ItemDefinition;

typedef struct Message{
  char character_name[32];
  char text[256];
  int id;
  int nextid;
} Message;

typedef struct Enemy{
    char species_name[32];
    Texture2D sprite;
    int hp;
}Enemy;

typedef struct Plant{
  char species_name[32];
  Texture2D sprite;
  int frameheight;
  int framewidth;
  int hitboxheight;
  int hitboxwidth;
  uint32_t default_trait_flags;
} Plant;

typedef struct Portal{
    int id;
    char level_name[32];
    Texture2D sprite;
} Portal;

typedef enum {
    TILE_WATER,TILE_GRASS ,TILE_YELLOW_OCHRE, TILE_DEEP_MOSS,TILE_STONE,
    TILE_ROAD, TILE_TERRA_PALE,TILE_SILT,TILE_MOON_STONE,TILE_PURPLE_OCHRE,
    TILE_BUFF_TITANIUM,TILE_POTTERS_PINK,TILE_SALMON,TILE_CELADON,TILE_MAY_GREEN,
    TILE_RED_OCHRE,TILE_SAGE, TILE_FIRED_GOLD_OCHRE,TILE_BASALT,TILE_BLUE_OCHRE,TILE_BERYL,TILE_AMAZONITE,
    TILE_WITHERED_LILAC,TILE_SHELL_PINK,TILE_BRILLIANT_JAUNE,TILE_NAPLES_YELLOW,TILE_NICKEL_TITANITE,TILE_BURNT_SIENNA} TileType;

typedef struct TileDefinition{
    int id;             // 0 = Sand, 1 = Asphalt, 2 = Magnetic Pit
    bool is_blocking;   //
    float friction;     // 1.0 = Normal, 0.2 = Ice/Oil, 1.5 = Deep Sand
    int footstep_sfx;
    Color color;// sound??
    char* label;
} TileDefinition;

typedef enum EntityType{
    ENTITY_CHARACTER,
    ENTITY_ITEM,
    ENTITY_PLANT,
    ENTITY_DECOR,
    ENTITY_MINERAL,
    ENTITY_PORTAL,
    ENTITY_ENEMY,
    ENTITY_PLAYER
} EntityType;

typedef enum TraitFlags{
  TRAIT_NONE = 0,
  TRAIT_TALK = 1 << 0,
  TRAIT_GATHER = 1 << 1,
  TRAIT_TELEPORT = 1 <<2, // used to designate that a enity can change the map
} TraitFlags;

typedef struct Character{
  char name[32];
  Texture2D sprite;
  int dialogId;
  uint32_t default_trait_flags;
} Character;

typedef struct Inventory{
  int itemIds[MAX_SLOTS];
  int count;
} Inventory;

typedef struct EquipmentSet{
    int weapon_id;
    int accessory_ids[MAX_ACCESSORY_SLOTS];
}EquipmentSet;

typedef struct player{
  Inventory inventory;
  int mineral_inventory[MINERAL_COUNT];
  float speed;
  Texture2D sprite;     // How fast we move
  StatBlock stats;
  EquipmentSet gear;
} Player;

#endif
