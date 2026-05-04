#include "registry/mineral_register.h"
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
    }
};

void DrawMineral(int id, Vector2 position){
    if (id < 0 || id >= MINERAL_COUNT) return; // Bounds check
    if (mineral_register[id].draw_function == NULL) return; // NULL check
    mineral_register[id].draw_function(position);
}
