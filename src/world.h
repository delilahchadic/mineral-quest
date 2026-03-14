#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "engine/palette.h"
#include <stdbool.h>

#define MAP_WIDTH 20
#define MAP_HEIGHT 15
#define TILE_SIZE 40

typedef struct Tile {
    int id;            // 0: Grass, 1: Wall/Tree, etc.
    bool is_blocking;  // Can the player walk here?
    Color color;       // Temporary until we load actual textures
} Tile;

typedef struct World {
    int tiles[MAP_HEIGHT][MAP_WIDTH];
    Tile types[2];     // Definitions for our tile types
} World;

// Functions
World Init_World();
void Draw_World(World* world);
bool Check_Collision(World* world, Vector2 nextPos);

#endif