#include "registry/mineral_register.h"
#include "defs/types_entities.h"
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
        .label = "Ruby",
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
    [MINERAL_ROSE_QUARTZ]={
        .label = "Rose Quartz",
        .color = COLOR_POTTERS_PINK,
        .draw_function = DrawRoseQuartz
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
    }
};

void DrawMineral(int id, Vector2 position){
    if (id < 0 || id >= MINERAL_COUNT) return; // Bounds check
    if (mineral_register[id].draw_function == NULL) return; // NULL check
    mineral_register[id].draw_function(position);
}
