#ifndef TYPES_TILES_H
#define TYPES_TILES_H
#include "raylib.h"
#include <stddef.h>
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
#endif
