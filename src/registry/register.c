#include "registry/register.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/mineral_register.h"

Enemy ENEMY_REGISTRY[100] = {0};
ItemDefinition ITEM_REGISTRY[100] = {0};
Message DIALOG_REGISTRY[500] = {0};
Character CHARACTER_REGISTRY[200] = {0};
Texture2D SPRITE_OVERRIDE[10] = {0};
Plant PLANT_REGISTRY[100] = {0};
Portal PORTAL_REGISTRY[100] = {0};
ExchangeNode NODE_REGISTRY[100] = {0};
Exchange EXCHANGE_REGISTRY[1000] = {0};
Sound MINERAL_SOUND;
Player GLOBAL_PLAYER;
Player *PLAYER;

int WORLD_COUNT = 0;
World WORLD_REGISTER[100] = {};
static int item_count = 0;
static int plant_count = 0;
static int charcter_count = 0;
static int portal_count = 0;
static int enemy_count = 0;

Texture2D PORTAL_TV_SPRITE;
Texture2D PORTAL_CRYSTAL_SPRITE;
Texture2D RUSTY_HEADSHOT_SPRITE;

char *STATS_NAMES[STAT_COUNT] = {
    "Strength", "Defense", "Magic Offense", "Magic Defense", "Speed",
    "Geology",  "Botany",  "Alchemy",       "Aerobics",      "Accessories"};

TileDefinition TILE_REGISTRY[42] = {
    // --- ROW 1: LIGHTS, BUFFS & PALE YELLOWS ---
    {TILE_LIMESTONE_CHALK, false, 1.0f, 114, COLOR_LIMESTONE_CHALK,
     "Limestone"},
    {TILE_SNOOT_PINK, false, 1.0f, 115, COLOR_SNOOT_PINK, "Snoot Pink"},
    {TILE_BUFF_TITANIUM, false, 0.8f, 104, COLOR_BUFF_TITANIUM,
     "Buff Titanium"},
    {TILE_STONE, false, 0.9f, 105, COLOR_TEXAS_HAZE, "Stone"},
    {TILE_BRILLIANT_JAUNE, false, 1.0f, 107, COLOR_BRILLIANT_JAUNE,
     "Brilliant Jaune"},
    {TILE_NAPLES_YELLOW, false, 1.0f, 107, COLOR_NAPLES_YELLOW,
     "Naples Yellow"},
    {TILE_NICKEL_TITANITE, false, 0.8f, 104, COLOR_NICKEL_TITANITE,
     "Nickel Titanite"},

    // --- ROW 2: ROSES, PINKS, SALMON & MAUVES ---
    {TILE_RHYOLITE_TUFF, false, 0.8f, 109, COLOR_RHYOLITE_TUFF,
     "Rhyolite Tuff"},
    {TILE_TERRA_PALE, false, 1.0f, 107, COLOR_TERRA_PALE, "Terra Pale"},
    {TILE_SHELL_PINK, false, 1.0f, 107, COLOR_SHELL_PINK, "Shell Pink"},
    {TILE_SALMON, false, 1.0f, 106, COLOR_DUSTY_SALMON, "Salmon"},
    {TILE_POTTERS_PINK, false, 0.9f, 105, COLOR_POTTERS_PINK, "Potters Pink"},
    {TILE_DUSTY_MAGENTA, false, 0.9f, 121, COLOR_DUSTY_MAGENTA,
     "Dusty Magenta"},
    {TILE_MUTED_FUSCHIA, false, 0.6f, 116, COLOR_MUTED_FUCHSIA,
     "Muted Fuschia"},
    {TILE_WITHERED_LILAC, false, 1.0f, 107, COLOR_WITHERED_LILAC,
     "Withered Lilac"},
    {TILE_CAPUT_MORTUUM, false, 0.8f, 113, COLOR_CAPUT_MORTUUM,
     "Caput Mortuum"},

    // --- ROW 3: EARTHS, STONES, UMBER & SIENNA ---

    {TILE_GRANITE, false, 1.0f, 117, COLOR_GRANITE_MID, "Granite"},
    {TILE_RAW_UMBER, false, 0.8f, 119, COLOR_RAW_UMBER, "Raw Umber"},
    {TILE_BURNT_SIENNA, false, 1.0f, 107, COLOR_BURNT_SIENNA, "Burnt Sienna"},
    {TILE_HEMATITE_BASE, false, 1.0f, 120, COLOR_HEMATITE_BASE,
     "Hematite Base"},
    {TILE_ROAD, false, 1.0f, 106, COLOR_ASPHALT, "Road"},

    // --- ROW 4: OCHRES & KHMEAR SANDSTONE ---
    {TILE_SILT, false, 1.0f, 108, COLOR_OCHRE_SILT, "Ochre Silt"},
    {TILE_YELLOW_OCHRE, false, 0.4f, 103, COLOR_YELLOW_OCHRE, "Yellow Ochre"},
    {TILE_FIRED_GOLD_OCHRE, false, 0.9f, 105, COLOR_FIRED_GOLD_OCHRE,
     "Fired Gold Ochre"},
    {TILE_RED_OCHRE, false, 1.0f, 108, COLOR_RED_OCHRE, "Red Ochre"},
    {TILE_PURPLE_OCHRE, false, 0.4f, 103, COLOR_PURPLE_OCHRE, "Purple Ochre"},
    {TILE_BLUE_OCHRE, false, 1.0f, 107, COLOR_BLUE_OCHRE, "Blue Ochre"},
    {TILE_KHMER_SANDSTONE, false, 1.0f, 112, COLOR_KHMER_SANDSTONE,
     "Khmer Sandstone"},

    // --- ROW 5: GREENS, SAGE & MOSS ---
    {TILE_GREEN_GOLD, false, 1.0f, 108, COLOR_GREEN_GOLD, "Green Gold"},
    {TILE_SAGE, false, 0.8f, 104, COLOR_SAGE_BRUSH, "Sage"},
    {TILE_CELADON, false, 1.0f, 107, COLOR_CELADON, "Celadon"},
    {TILE_WITHERED_VIRIDIAN, false, 1.0f, 122, COLOR_WITHERED_VIRIDIAN,
     "Withered Viridian"},
    {TILE_GRASS, false, 1.0f, 102, COLOR_DUSTY_SAP, "Grass"},
    {TILE_OLIVE_DRAB, false, 0.9f, 118, COLOR_OLIVE_DRAB, "Olive Drab"},
    {TILE_DEEP_MOSS, false, 1.0f, 107, COLOR_DEEP_MOSS, "Deep Moss"},
    {TILE_AMAZONITE, false, 1.0f, 107, COLOR_AMAZONITE, "Amazonite"},

    // --- ROW 6: BLUES, TEALS, SLATES & DARK INORGANICS ---
    {TILE_COBALT_TEAL_PALE, false, 0.7f, 110, COLOR_COBALT_TEAL_PALE,
     "Cobalt Teal Pale"},
    {TILE_BERYL, false, 1.0f, 107, COLOR_CERULEAN_BERYL, "Beryl"},

    {TILE_LIVID_SLATE, false, 0.9f, 111, COLOR_LIVID_SLATE, "Livid Slate"},
    {TILE_MOON_STONE, false, 1.0f, 102, COLOR_MOONGLOW, "Moon Glow"},
    {TILE_BASALT, false, 1.0f, 106, COLOR_BASALT, "Basalt"},
    {TILE_WATER, true, 0.5f, 101, COLOR_BEAVIS_SHIRT, "Water"}};

char *GetNodeName(int node_id) { return NODE_REGISTRY[node_id].name; }
char *GetName(EntityType type, int id) {
    switch (type) {
    case ENTITY_PLANT:
        return PLANT_REGISTRY[id].species_name;
        break;
    case ENTITY_CHARACTER:
        return CHARACTER_REGISTRY[id].name;
        break;
    case ENTITY_ITEM:
        return ITEM_REGISTRY[id].name;
        break;
    case ENTITY_PORTAL:
        return PORTAL_REGISTRY[id].name;
        break;
    case ENTITY_ENEMY:
        return ENEMY_REGISTRY[id].species_name;
    case ENTITY_MINERAL:
        return GetMineralLabel(id);
    default:
        return NULL;
    }
}

StatBlock *GetStats(EntityType type, int id) {
    (void)id;
    switch (type) {
    case ENTITY_PLAYER:
        return &GLOBAL_PLAYER.stats;
    default:
        return NULL;
    }
}

int GetAccesorySlot(EntityType type, int id) {
    switch (type) {
    case ENTITY_ITEM:
        return ITEM_REGISTRY[id].slot;
    default:
        return -1;
    }
}
char *GetDescription(EntityType type, int id) {
    switch (type) {
    case ENTITY_ITEM:
        return ITEM_REGISTRY[id].description;
    default:
        return NULL;
    }
}

Texture2D *GetSpriteOverride(int id) { return &SPRITE_OVERRIDE[id]; }

Texture2D *GetSprite(EntityType type, int id) {
    switch (type) {
    case ENTITY_PLANT:
        return &PLANT_REGISTRY[id].sprite;
        break;
    case ENTITY_CHARACTER:
        return &CHARACTER_REGISTRY[id].sprite;
    case ENTITY_ITEM:
        return GetSpriteOverride(0);
    case ENTITY_PLAYER:
        return &GLOBAL_PLAYER.sprite;
    case ENTITY_PORTAL:
        switch (PORTAL_REGISTRY[id].type) {
        case PORTAL_CRYSTAL:
            return &PORTAL_CRYSTAL_SPRITE;
        case PORTAL_TV:
            return &PORTAL_TV_SPRITE;
        }
    case ENTITY_ENEMY:
        return &ENEMY_REGISTRY[id].sprite;
    default:
        return NULL;
    }
}

int GetDialogID(EntityType type, int id) {
    switch (type) {
    case ENTITY_CHARACTER:
        return CHARACTER_REGISTRY[id].dialogId;
    default:
        return -1;
    }
}

uint32_t GetDefaultTraitFlags(EntityType type, int id) {
    switch (type) {
    case ENTITY_PLANT:
        return PLANT_REGISTRY[id].default_trait_flags;
        break;
    case ENTITY_CHARACTER:
        return CHARACTER_REGISTRY[id].default_trait_flags;
    case ENTITY_ITEM:
        return TRAIT_GATHER;
    case ENTITY_PORTAL:
        return TRAIT_TELEPORT;
    default:
        return TRAIT_NONE;
    }
}

int GetEntityTypeCount(EntityType type) {
    switch (type) {
    case ENTITY_PLANT:
        return plant_count;
    case ENTITY_ITEM:
        return item_count;
    case ENTITY_CHARACTER:
        return charcter_count;
    case ENTITY_PORTAL:
        return portal_count;
    case ENTITY_ENEMY:
        return enemy_count;
    case ENTITY_MINERAL:
        return MINERAL_COUNT;
    case ENTITY_PLAYER:
        return 1;
    default:
        return -1;
    }
}

void SetEntityTypeCount(EntityType type, int count) {
    switch (type) {
    case ENTITY_PLANT:
        plant_count = count;
        return;
    case ENTITY_ITEM:
        item_count = count;
        return;
    case ENTITY_CHARACTER:
        charcter_count = count;
        return;
    case ENTITY_PORTAL:
        portal_count = count;
        return;
    case ENTITY_ENEMY:
        enemy_count = count;
        return;
    default:
        return;
    }
}

int GetCharacterId(int node_id) { return NODE_REGISTRY[node_id].character_id; }

char *GetWorldName(int world_id) {
    if (world_id < 0 || world_id > 99 || world_id >= WORLD_COUNT) {
        return "invalid world name";
    }
    return WORLD_REGISTER[world_id].name;
}
char *GetWorldNameFromPortalId(int portal_id) {
    if (portal_id < 0 || portal_id > 99 || portal_id >= portal_count) {
        return "invalid world name";
    }
    return WORLD_REGISTER[PORTAL_REGISTRY[portal_id].world_id].name;
}

int GetWorldIDFromPortal(int portal_id) {
    if (portal_id < 0 || portal_id > 99 || portal_id >= portal_count) {
        return -1;
    }
    return PORTAL_REGISTRY[portal_id].world_id;
}
PortalType GetPortalType(int id) { return PORTAL_REGISTRY[id].type; }
Vector2 GetDestination(EntityType type, int id) {
    switch (type) {
    case ENTITY_PORTAL:
        return PORTAL_REGISTRY[id].destination;
    default:
        return (Vector2){0, 0};
    }
}

Vector2 GetEntityCenter(MapEntity *entity) {
    Texture2D *texture = GetSprite(entity->type, entity->entity_id);
    return (Vector2){entity->position.x + (texture->width / 2.0f),
                     entity->position.y + (texture->height / 2.0f)};
}
