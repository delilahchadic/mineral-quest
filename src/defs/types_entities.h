#ifndef TYPES_ENTITIES
#define TYPES_ENTITIES

#include "stdint.h"
#include "raylib.h"
#include "defs/constants.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"

typedef enum ItemType{
  ITEM_VHS_TAPE,
  ITEM_TAROT_CARD,
  ITEM_MISC,
  ITEM_KEY_ITEM,
  ITEM_MINERAL,
  ITEM_EQUIP,
  ITEM_CONSUME
} ItemType;

typedef enum {
    USE_NONE = 0,
    USE_RESTORE_HP,
    USE_TEMP_BUFF,
    USE_PERM_BOOST
} UseType;

typedef enum {
    SLOT_NONE = -1,     // Non-equipables (Consumables, Minerals, Key Items)
    SLOT_WEAPON = 0,    // Primary weapon
    SLOT_ACCESSORY,
    SLOT_TAROT,// Accessories (capped by player stats)
    SLOT_COUNT          // Total valid equipment slot types (2)
} EquipSlot;

typedef struct ItemDefinition {
    int id;
    char name[32];
    char description[128];
    int type;
    UseType use_type;
    float use_duration;
    EquipSlot slot;                   // Explicit enum type instead of raw int
    int hp_bonus;
    int mp_bonus;
    int stat_bonuses[STAT_COUNT];     // Matches all 9 stats (STR through AEROBICS)
    int granted_ability_id;           // -1 if no ability attached
} ItemDefinition;

typedef struct Enemy{
    char species_name[32];
    Texture2D sprite;
    int hp;
}Enemy;

typedef struct Recipe{
    char name[32];
    int id;
    int exchangeId;
}Recipe;

typedef struct Plant{
  char species_name[32];
  Texture2D sprite;
  int frameheight;
  int framewidth;
  int hitboxheight;
  int hitboxwidth;
  int gather_level;
  int damage_amount;
  int damage_waive_level;
  uint32_t default_trait_flags;
  int hp_bonus;
  int mp_bonus;
  int stat_bonuses[STAT_COUNT];
  int cost;
} Plant;

typedef struct Projectile{
    Texture2D sprite;
}Projectile;

typedef enum PortalType{
    PORTAL_TV,
    PORTAL_CRYSTAL
}PortalType;

typedef struct World{
    int id;
    char name[32];
}World;

typedef struct Portal{
    int id;
    char name[32];
    int world_id;
    PortalType type;
    Vector2 destination;
} Portal;

typedef enum EntityType{
    ENTITY_NONE=-1,
    ENTITY_CHARACTER,
    ENTITY_ITEM,
    ENTITY_PLANT,
    ENTITY_DECOR,
    ENTITY_MINERAL,
    ENTITY_PORTAL,
    ENTITY_ENEMY,
    ENTITY_PLAYER,
    ENTITY_PROJECTILE,
    ENTITY_RECIPE
} EntityType;

typedef enum TraitFlags{
  TRAIT_NONE = 0,
  TRAIT_TALK = 1 << 0,
  TRAIT_GATHER = 1 << 1,
  TRAIT_TELEPORT = 1 <<2, // used to designate that a enity can change the map
  TRAIT_NODE = 1 <<3, // used to designate characters that associated to a node
  TRAIT_DAMAGE = 1 <<4,
} TraitFlags;

typedef enum CharacterType{
    CHARACTER_DEFAULT = 0,
    CHARACTER_NODE
}CharacterType;

typedef struct Character{
  CharacterType type;
  char name[32];
  Texture2D sprite;
  int dialogId;
  uint32_t default_trait_flags;
} Character;

typedef enum ExchangeNodeType{
    NODE_SHOP,
    NODE_QUEST,
    NODE_TEMPLE,
    NODE_FORGE
} ExchangeNodeType;

typedef struct {
    EntityType type;
    int id;
    int amount;
} CostSlot;

typedef struct Exchange{
    int item_id;
    CostSlot cost_slots[3];
}Exchange;

typedef struct ExchangeNode{
    int id;
    int character_id;
    int reset_cycle;
    char name[32];
    int exchange_ids[10];
    int quantities[10];
    int count;
    ExchangeNodeType type;
}ExchangeNode;

#endif
