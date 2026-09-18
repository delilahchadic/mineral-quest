#ifndef TYPES_ENTITIES
#define TYPES_ENTITIES

#include "stdint.h"
#include "raylib.h"
#include "defs/constants.h"
#include "defs/types_minerals.h"

#define MAX_ACTIVE_BUFFS 8

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

typedef struct {
    int id;
    int hpBonus;
    int modifiers[STAT_COUNT];   // Holds +2 Aerobics, +1 Grace, -1 Defense, etc.
    float duration;              // Remaining time in seconds
    bool active;
} ActiveBuff;

typedef struct StatBlock {
    int base[STAT_COUNT];
    int current[STAT_COUNT];
    ActiveBuff buffs[MAX_ACTIVE_BUFFS];
    int max_hp;
    int current_hp;
} StatBlock;

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
    SLOT_ACCESSORY,     // Accessories (capped by player stats)
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

typedef enum {
    // --- ROW 1: LIGHTS, BUFFS & PALE YELLOWS ---
    TILE_LIMESTONE_CHALK,
    TILE_SNOOT_PINK,
    TILE_BUFF_TITANIUM,
    TILE_STONE,
    TILE_BRILLIANT_JAUNE,
    TILE_NAPLES_YELLOW,
    TILE_NICKEL_TITANITE,

    // --- ROW 2: ROSES, PINKS, SALMON & MAUVES ---
    TILE_RHYOLITE_TUFF,
    TILE_TERRA_PALE,
    TILE_SHELL_PINK,
    TILE_SALMON,
    TILE_POTTERS_PINK,
    TILE_DUSTY_MAGENTA,
    TILE_MUTED_FUSCHIA,
    TILE_WITHERED_LILAC,
    TILE_CAPUT_MORTUUM,

    // --- ROW 3: EARTHS, STONES, UMBER & SIENNA ---
    TILE_GRANITE,
    TILE_RAW_UMBER,
    TILE_BURNT_SIENNA,
    TILE_HEMATITE_BASE,
    TILE_ROAD,

    // --- ROW 4: OCHRES & KHMER SANDSTONE ---
    TILE_SILT,
    TILE_YELLOW_OCHRE,
    TILE_FIRED_GOLD_OCHRE,
    TILE_RED_OCHRE,
    TILE_PURPLE_OCHRE,
    TILE_BLUE_OCHRE,
    TILE_KHMER_SANDSTONE,

    // --- ROW 5: GREENS, SAGE & MOSS ---
    TILE_GREEN_GOLD,
    TILE_SAGE,
    TILE_CELADON,
    TILE_WITHERED_VIRIDIAN,
    TILE_GRASS,
    TILE_OLIVE_DRAB,
    TILE_DEEP_MOSS,
    TILE_AMAZONITE,

    // --- ROW 6: BLUES, TEALS, SLATES & DARK INORGANICS ---
    TILE_COBALT_TEAL_PALE,
    TILE_BERYL,
    TILE_LIVID_SLATE,
    TILE_MOON_STONE,
    TILE_BASALT,
    TILE_WATER,

    TILE_COUNT // Useful for array sizing or loops
} TileType;

typedef struct TileDefinition{
    int id;             // 0 = Sand, 1 = Asphalt, 2 = Magnetic Pit
    bool is_blocking;   //
    float friction;     // 1.0 = Normal, 0.2 = Ice/Oil, 1.5 = Deep Sand
    int footstep_sfx;
    Color color;// sound??
    char* label;
} TileDefinition;

typedef enum EntityType{
    ENTITY_NONE=-1,
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
  TRAIT_NODE = 1 <<3, // used to designate characters that associated to a node
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

typedef struct EquipmentSet{
    int weapon_id;
    int accessory_ids[MAX_ACCESSORY_SLOTS];
}EquipmentSet;

typedef struct Player{
  int item_inventory[100];
  int mineral_inventory[MINERAL_COUNT];
  int plant_inventory[100];
  float speed;
  Texture2D sprite;     // How fast we move
  StatBlock stats;
  EquipmentSet gear;
} Player;

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
