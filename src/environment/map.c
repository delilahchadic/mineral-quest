#include "map.h"

#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "systems/input.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "systems/behavior.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum ElementType {
    ELEMENT_NONE,
    ELEMENT_FIRE,
    ELEMENT_WATER
} ElementType;

void InitMap(Map *map) {
    InitPlayer(map);
    map->lastTileHeight = -1;
    map->pixel_width = map->columns * TILE_SIZE;
    map->pixel_height = map->rows * TILE_SIZE;
    map->is_ready = true;
}

void InitNewMap(Map *map, char *name, int columns, int rows) {
    memset(map, 0, sizeof(Map));
    snprintf(map->name, sizeof(map->name), "%s", name);
    map->name[sizeof(map->name) - 1] = '\0';
    map->rows = rows;
    map->columns = columns;
    for (int i = 0; i < map->rows; i++) {
        for (int j = 0; j < map->columns; j++) {
            map->grid[i][j].height = 0;
            map->grid[i][j].type = TILE_GRASS;
            map->grid[i][j].isoPos =
                GetWorldToIso((Vector2){j * TILE_SIZE, i * TILE_SIZE});
        }
    }

    map->pixel_width = map->columns * TILE_SIZE;
    map->pixel_height = map->rows * TILE_SIZE;
    map->is_ready = true;
}

void InitPlayer(Map *map) {

    MapEntity *player = &map->player;
    if (player == NULL)
        return;

    player->type = ENTITY_PLAYER;
    player->state = NORMAL_STATE;
    player->jumpoffset = 0.0f;
    player->combat.isAttacking = false;
    player->combat.attackTimer = 0;
    player->combat.attackDuration = 0.25f;
    player->stats = GetStats(ENTITY_PLAYER, 0);
    player->instance_id = -1;
}

void Close_Map(Map *map) {
    if (map == NULL) {
        return;
    }

    BuildingZone *b = NULL;
    b = map->buildings;
    while (b != NULL) {
        map->buildings = b->next;
        free(b);
        b = map->buildings;
    }
}
// 1. Converts World Coordinates (e.g. Player position) to Screen Isometric
Vector2 GetWorldToIso(Vector2 worldPos) {
    Vector2 iso;
    iso.x = (worldPos.x - worldPos.y);
    iso.y = (worldPos.x + worldPos.y) / 2.0f;
    return iso;
}

// 2. Converts Screen Isometric pixels back to Grid Indices (e.g. x=1, y=2)
Vector2 GetIsoWorldToGrid(Vector2 worldPos) {
    Vector2 grid;
    float tileW = (float)TILE_SIZE;
    float tileH = (float)TILE_SIZE / 2.0f;

    grid.x = (worldPos.x / tileW + worldPos.y / tileH) / 2.0f;
    grid.y = (worldPos.y / tileH - worldPos.x / tileW) / 2.0f;
    return grid;
}

// 3. Converts Grid Indices directly to Screen Isometric pixels
Vector2 GetGridToIsoWorld(int x, int y) {
    // FIXED: X-axis now uses TILE_SIZE to perfectly match GetWorldToIso
    float worldX = (x - y) * (float)TILE_SIZE;
    float worldY = (x + y) * ((float)TILE_SIZE / 2.0f);
    return (Vector2){worldX, worldY};
}

Vector2 GetIsoWorldToGridWithHeight(Map *map, Vector2 screenWorldPos) {
    // 1. Define your max height (matches your grid limits)
    // If your max height is 10, start there.
    const int MAX_HEIGHT = 150;
    const float HEIGHT_STEP = 8.0f; // From your Draw_Tile: height * 8.0f

    for (int h = MAX_HEIGHT; h >= 0; h--) {
        // 2. Offset the Y coordinate to 'drop' the screen click
        // to the level of the current height slice.
        float pixelOffset = h * HEIGHT_STEP;
        Vector2 testPos = {screenWorldPos.x, screenWorldPos.y + pixelOffset};

        // 3. Use your existing math to find what grid cell that corresponds to
        Vector2 grid = GetIsoWorldToGrid(testPos);

        int ix = (int)grid.x;
        int iy = (int)grid.y;

        // 4. Validate the grid index
        if (ix >= 0 && ix < map->columns && iy >= 0 && iy < map->rows) {
            // 5. Check: Is the tile at this grid coordinate actually at this
            // height? We check >= because you might be clicking the "side" of a
            // tall block.
            if (map->grid[iy][ix].height >= h) {
                return (Vector2){(float)ix, (float)iy};
            }
        }
    }

    // Fallback: If nothing was hit, return the flat ground (height 0)
    return GetIsoWorldToGrid(screenWorldPos);
}

void ResetAllHitFlags(Map *map) {
    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *e = &map->entities[i];
        e->hitThisSwing = false;
    }
}

void AddBuilding(Map *map, BuildingZone *building) {
    if (building == NULL)
        return;
    building->next = map->buildings;
    map->buildings = building;
    map->building_id++;
}

void Remove_Building(Map *map, BuildingZone *building) {
    if (map == NULL || building == NULL)
        return;

    if (map->buildings == building) {
        map->buildings = building->next;
        building->next = NULL;
        return;
    }

    BuildingZone *curr = map->buildings;
    while (curr->next != NULL && curr->next != building) {
        curr = curr->next;
    }
    if (curr->next == building) {
        curr->next = building->next;
        building->next = NULL;
        free(building);
        return;
    }
}

void RemoveEntityAt(Map *map, int index) {
    if (index < 0 || index >= map->entity_count)
        return;

    // If the entity being removed isn't already the last one,
    // overwrite its slot with the last entity in the array.
    if (index < map->entity_count - 1) {
        map->entities[index] = map->entities[map->entity_count - 1];
    }

    // Shrink the active count by 1
    map->entity_count--;
}

MapEntity *AddEntity(Map *map) {
    if (map->entity_count >= MAX_ENTITIES) {
        return NULL;
    }
    MapEntity *slot = &map->entities[map->entity_count];
    memset(slot, 0, sizeof(MapEntity));

    // Automatically handle unique instance ID here!
    slot->instance_id = map->entitity_id++;

    map->entity_count++;
    return slot;
}

int PollTrait(Map *map, TraitFlags trait, float distance) {
    Vector2 playerCenter = GetEntityCenter(&map->player);

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *tmp = &map->entities[i];
        if (tmp->trait_flags & trait) {
            Vector2 targetCenter = GetEntityCenter(tmp);

            if (Vector2Distance(playerCenter, targetCenter) < distance) {
                return i;
            }
        }
    }
    return -1;
}

int getExchangeNodeByCharacterId(
    Map *map, int character_id) { // poll map->nodes for a node that has a
                                  // matching character id
    if (map == NULL || map->node_count < 0 || character_id < 0)
        return -1;

    for (int i = 0; i < map->node_count; i++) {
        int owner_id = GetCharacterId(map->active_nodes[i]);
        if (owner_id == character_id)
            return map->active_nodes[i];
    }
    return -1;
}
