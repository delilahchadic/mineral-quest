#include "world.h"

World Init_World() {
    World world = {0};

    // Define Tile Types
    world.types[0] = (Tile){ .id = 0, .is_blocking = false, .color = WHITE }; // Grass
    world.types[1] = (Tile){ .id = 1, .is_blocking = true,  .color = MAROON }; // Wall
    // world.types[12] = (Tile){ .id = 1, .is_blocking = true,  .color = Aqu };
    // Create a simple room with a border
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                world.tiles[y][x] = 1; // Wall
            } else {
                world.tiles[y][x] = 0; // Grass
            }
        }
    }
    return world;
}

void Draw_World(World* world) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            Tile t = world->types[world->tiles[y][x]];
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, t.color);
            // Draw a subtle grid line
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Fade(BLACK, 0.1f));
        }
    }
}

bool Check_Collision(World* world, Vector2 nextPos) {
    // Convert pixel coordinates to grid coordinates
    int tileX = (int)(nextPos.x / TILE_SIZE);
    int tileY = (int)(nextPos.y / TILE_SIZE);

    // Bounds check to prevent crashing if player leaves the map
    if (tileX < 0 || tileX >= MAP_WIDTH || tileY < 0 || tileY >= MAP_HEIGHT) return true;

    // Check if the tile at that position is blocking
    return world->types[world->tiles[tileY][tileX]].is_blocking;
}