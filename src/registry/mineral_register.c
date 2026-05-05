#include "registry/mineral_register.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "engine/palette.h"
#include <stddef.h>
#include "systems/minerals/mineral_draw_functions.h"
#include "raylib.h"
#include "systems/minerals/mineral_draw_functions.h"
MineralDefinition mineral_register[MINERAL_COUNT] = {
    [MINERAL_RUBY] = {
        .label = "Ruby",
        .color = COLOR_NEON_CRIMSON,
        .draw_function = DrawRuby
    },
    [MINERAL_SAPHIRE]={
        .label = "Saphire",
        .color = COLOR_COBALT_BLUE,
       .draw_function = DrawSaphire
    },
    [MINERAL_PYRITE]={
        .label = "Pyrite",
        .color = COLOR_PYRITE_BRASS,
        .draw_function = DrawPyrite
    },
    [MINERAL_PEARL]={
        .label = "Pearl",
        .color = WHITE,
        .draw_function = DrawPearl
    },
    [MINERAL_QUARTZ]={
        .label = "Quartz",
        .color = WHITE,
        .draw_function = DrawQuartz
    },
    [MINERAL_ROSE_QUARTZ]={
        .label = "Rose Quartz",
        .color = COLOR_POTTERS_PINK,
        .draw_function = DrawRoseQuartz
    },
    [MINERAL_AMETHYST]={
        .label = "Amethyst",
        .color = COLOR_PURPLE_OCHRE,
        .draw_function = DrawAmethyst
    },
    [MINERAL_FELDSPAR]={
        .label = "Feldspar",
        .color = COLOR_DUSTY_SALMON,
        .draw_function = DrawOrthoclase
    },
    [MINERAL_EMERALD]={
        .label = "Emerald",
        .color = COLOR_JADE,
        .draw_function = DrawEmerald
    },
    [MINERAL_TOPAZ]={
        .label = "Topaz",
        .color = COLOR_NAPLES_YELLOW,
        .draw_function = DrawTopaz
    },
    [MINERAL_MAGNETITE]={
        .label = "Magnetite",
        .color = COLOR_SUNKEN_INK,
        .draw_function = DrawMagnetite
    },
    [MINERAL_PLAGIOCASE]={
        .label = "Plagiocase",
        .color = COLOR_BLUE_OCHRE,
        .draw_function = DrawPlagiocase
    },
    [MINERAL_MICA]={
        .label = "Mica",
        .color = WHITE,
        .draw_function = DrawMuscovite
    },
    [MINERAL_BIOTITE]={
        .label = "Biotite",
        .color = COLOR_SUNKEN_INK,
        .draw_function = DrawBiotite
    },
    [MINERAL_OLIVINE]={
        .label = "Olivine",
        .color = COLOR_JADE,
        .draw_function = DrawOlivine
    },
    [MINERAL_BERYL]={
        .label = "Beryl",
        .color = COLOR_CERULEAN_BERYL,
        .draw_function = DrawBeryl
    },
    [MINERAL_HALITE]={
        .label = "Halite",
        .color = WHITE,
        .draw_function = DrawHalite
    }
};

void DrawMineral(int id, Vector2 position){
    if (id < 0 || id >= MINERAL_COUNT) return; // Bounds check
    if (mineral_register[id].draw_function == NULL) return; // NULL check
    mineral_register[id].draw_function(position);
}

char* GetMineralLabel(int id){
    return mineral_register[id].label;
}

Color GetMineralColor(int id){
    if (id < 0 || id >= MINERAL_COUNT) return WHITE; // Bounds check
    return mineral_register[id].color;
}
