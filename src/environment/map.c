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
    // map->player = player;
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
    // Check if your array is full
    if (map->entity_count >= MAX_ENTITIES) {
        return NULL; // Array capacity reached!
    }
    // Grab a pointer directly to the next empty slot in the array
    MapEntity *slot = &map->entities[map->entity_count];

    // Clear out old garbage data from that slot (like memset)
    memset(slot, 0, sizeof(MapEntity));

    // Bump up your active count–––
    map->entity_count++;

    return slot; // Return the pointer so you can set its fields
}

int PollTrait(Map *map, TraitFlags trait, float distance) {

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *tmp = &map->entities[i];
        if (tmp->trait_flags & trait) {
            if (Vector2Distance(map->player.position, tmp->position) <
                distance) {
                return i;
            }
        }
        // tmp = tmp->next;
    }
    return -1;
}

bool PickNewWanderTarget(Map *map, MapEntity *entity) {
    int cur_tx = (int)(entity->position.x / TILE_SIZE);
    int cur_ty = (int)(entity->position.y / TILE_SIZE);

    if (cur_tx < 0 || cur_tx >= map->columns || cur_ty < 0 ||
        cur_ty >= map->rows)
        return false;
    int cur_h = map->grid[cur_ty][cur_tx].height;

    for (int i = 0; i < 10; i++) {
        // Generate candidate around current position
        float cx = entity->position.x + ((rand() % 128) - 64);
        float cy = entity->position.y + ((rand() % 128) - 64);

        // Clamp to map boundaries so it never goes into negative space or
        // off-screen
        if (cx < 0.0f)
            cx = 0.0f;
        if (cx > (float)map->pixel_width)
            cx = (float)map->pixel_width;
        if (cy < 0.0f)
            cy = 0.0f;
        if (cy > (float)map->pixel_height)
            cy = (float)map->pixel_height;

        int t_tx = (int)(cx / TILE_SIZE);
        int t_ty = (int)(cy / TILE_SIZE);

        if (t_tx >= map->columns)
            t_tx = map->columns - 1;
        if (t_ty >= map->rows)
            t_ty = map->rows - 1;

        if (t_tx >= 0 && t_tx < map->columns && t_ty >= 0 && t_ty < map->rows) {
            if (abs(map->grid[t_ty][t_tx].height - cur_h) <= 2) {
                entity->target_position = (Vector2){cx, cy};
                return true;
            }
        }
    }
    return false;
}
void UpdateWanderBehavior(Map *map, MapEntity *entity, float dt) {
    Vector2 diff = Vector2Subtract(entity->target_position, entity->position);
    float distance = Vector2Length(diff);

    if (distance > 1.0f) {
        entity->position =
            Vector2Add(entity->position, Vector2Scale(Vector2Normalize(diff),
                                                      entity->speed * dt));

        // Hard clamp position to stay within map pixel boundaries
        if (entity->position.x < 0.0f)
            entity->position.x = 0.0f;
        if (entity->position.x > (float)map->pixel_width)
            entity->position.x = (float)map->pixel_width;
        if (entity->position.y < 0.0f)
            entity->position.y = 0.0f;
        if (entity->position.y > (float)map->pixel_height)
            entity->position.y = (float)map->pixel_height;

        int tx = (int)(entity->position.x / TILE_SIZE);
        int ty = (int)(entity->position.y / TILE_SIZE);

        // Prevent array out-of-bounds if exactly on the right/bottom edge
        if (tx >= map->columns)
            tx = map->columns - 1;
        if (ty >= map->rows)
            ty = map->rows - 1;

        if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
            float targetAlt = map->grid[ty][tx].height * 8.0f;
            entity->altitude += (targetAlt - entity->altitude) * 10.0f * dt;
        }
    } else if (!PickNewWanderTarget(map, entity)) {
        entity->target_position = entity->position;
    }
}

void UpdateEnemyCombat(Map *map, MapEntity *e, float dt) {
    if (e->type != ENTITY_ENEMY)
        return;

    MapEntity *player = &map->player;
    float distToPlayer = Vector2Distance(e->position, player->position);

    switch (e->behavior) {
    case BEHAVIOR_WANDER:
        // Existing wander behavior, but poll for player proximity
        if (distToPlayer < 140.0f) {
            e->behavior = BEHAVIOR_CHASE;
        } else {
            // Run normal wander movement
            UpdateWanderBehavior(map, e, dt);
        }
        break;

    case BEHAVIOR_CHASE:
        if (distToPlayer > 220.0f) {
            e->behavior = BEHAVIOR_WANDER;
        } else if (distToPlayer <
                   80.0f) { // Widen from 50.0f to catch clustered group
            // Close enough to strike! Enter telegraph/windup
            e->behavior = BEHAVIOR_WINDUP;
            e->behavior_timer = 0.0f;
        } else {
            // Move smoothly toward player
            Vector2 dir = Vector2Normalize(
                Vector2Subtract(player->position, e->position));
            e->position = Vector2Add(e->position,
                                     Vector2Scale(dir, e->speed * 0.9f * dt));
        }
        break;

    case BEHAVIOR_WINDUP:
        e->behavior_timer += dt;
        // 300ms visual telegraph window
        if (e->behavior_timer >= 0.3f) {
            e->behavior = BEHAVIOR_ATTACK;
            e->behavior_timer = 0.0f;
        }
        break;

    case BEHAVIOR_ATTACK: {
        // Give enemies a generous hit footprint matching their 64x64
        // transparent sprite bounds
        float enemyRadius = 26.0f;
        float playerRadius = 12.0f; // Adjust to your player core size
        Vector2 dir =
            Vector2Normalize(Vector2Subtract(player->position, e->position));
        e->position =
            Vector2Add(e->position, Vector2Scale(dir, e->speed * 0.9f * dt));
        // Check edge-to-edge distance instead of strict center-to-center
        if (distToPlayer < (enemyRadius + playerRadius + 10.0f)) {
            // Hurt player logic
            printf("Player hit by enemy!\n");
            map->hitstop_timer = 0.06f; // Impact hitstop
            DamagePlayer(35);

            // Apply knockback to player
            Vector2 kbDir = Vector2Normalize(
                Vector2Subtract(player->position, e->position));
            player->position =
                Vector2Add(player->position, Vector2Scale(kbDir, 25.0f));
            e->behavior = BEHAVIOR_RECOVERY;
            e->behavior_timer = 0.0f;
        }

        break;
    }

    case BEHAVIOR_RECOVERY:
        e->behavior_timer += dt;
        // 400ms recovery window where enemy is vulnerable
        if (e->behavior_timer >= 0.4f) {
            e->behavior = BEHAVIOR_CHASE;
        }
        break;

    default:
        e->behavior = BEHAVIOR_WANDER;
        break;
    }
}

void UpdateEntityMovement(Map *map, float dt) {
    if (map == NULL)
        return;

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *entity = &map->entities[i];

        if (entity->isCollecting) {
            Vector2 targetPos = map->player.position;
            entity->position =
                Vector2Lerp(entity->position, targetPos, 40.0f * dt);

            if (Vector2DistanceSqr(entity->position, targetPos) < 10.0f) {
                GLOBAL_PLAYER.mineral_inventory[entity->id]++;
                float pitch = 0.95f + ((float)(entity->id % 10) / 100.0f);
                SetSoundPitch(MINERAL_SOUND, pitch);
                PlaySound(MINERAL_SOUND);

                // Remove it using your flat-array helper, then step i back
                // so the newly shifted element at this index isn't skipped!
                RemoveEntityAt(map, i);
                i--;
                continue;
            }
        }

        if (entity->type == ENTITY_ENEMY) {
            UpdateEnemyCombat(map, entity, dt);
        } else if (entity->behavior == BEHAVIOR_WANDER) {
            UpdateWanderBehavior(map, entity, dt);
        }
    }
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
