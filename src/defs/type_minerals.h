#include "raylib.h"

typedef enum Mineral{
    MINERAL_RUBY = 0,
    MINERAL_SAPHIRE,
    MINERAL_PYRITE,
    MINERAL_PEARL,
    MINERAL_QUARTZ,
    MINERAL_FELDSPAR,
    MINERAL_COUNT
}Mineral;

typedef struct MineralDefinition{
    char label[32];
    Color color;
    void (*draw_function)(Vector2 position);
}MineralDefinition;
void(*draw_functions[100]) (void* context);
