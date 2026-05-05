#ifndef TYPES_MINERAL_H
#define TYPES_MINERAL_H
#include "raylib.h"

typedef enum Mineral{
    MINERAL_RUBY = 0,
    MINERAL_SAPHIRE,
    MINERAL_PYRITE,
    MINERAL_PEARL,
    MINERAL_QUARTZ,
    MINERAL_ROSE_QUARTZ,
    MINERAL_AMETHYST,
    MINERAL_FELDSPAR,
    MINERAL_EMERALD,
    MINERAL_TOPAZ,
    MINERAL_MAGNETITE,
    MINERAL_PLAGIOCASE,
    MINERAL_MICA,
    MINERAL_BIOTITE,
    MINERAL_OLIVINE,
    MINERAL_BERYL,
    MINERAL_HALITE,
    MINERAL_COUNT
}Mineral;

typedef struct MineralDefinition{
    char label[32];
    Color color;
    void (*draw_function)(Vector2 position);
}MineralDefinition;
void(*draw_functions[100]) (void* context);
#endif
