#include "map.h"

#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "raylib.h"
#include "raymath.h"
#include "registry/mineral_register.h"
#include "registry/register.h"
#include "systems/behavior.h"
#include "systems/input.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "systems/weapon_grid.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    AssignNewMovementComponent(map, player);
    MovementComponent *movement_component =
        &map->movement_components[player->movement_component_id];
    movement_component->state = NORMAL_STATE;
    movement_component->jumpoffset = 0.0f;
    player->instance_id = -2;
}

MovementComponent *GetPlayerMovementComponent(Map *map) {
    return &map->movement_components[map->player.movement_component_id];
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
    printf("Resetting hit flags\n");
    for (int i = 0; i < 250; i++) {
        if (map->combat_components[i].is_active) {
            map->combat_components[i].hitThisSwing = false;
        }
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
        free(building);
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

    MapEntity *entity = &map->entities[index];

    // Release the movement component back to the pool if it has one
    if (entity->movement_component_id != -1 &&
        entity->movement_component_id < 250) {
        map->movement_components[entity->movement_component_id].is_active =
            false;
        entity->movement_component_id = -1;
    }

    if (entity->combat_component_id != -1 &&
        entity->combat_component_id < 250) {
        map->combat_components[entity->combat_component_id].is_active = false;
        entity->combat_component_id = -1;
    }

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
    slot->instance_id = map->instance_id_count++;
    slot->movement_component_id = INVALID_INDEX;
    slot->combat_component_id = INVALID_INDEX;
    map->entity_count++;
    return slot;
}

int CheckTargetTrait(Player *player, Map *map, uint32_t trait) {
    int targetId = player->targeting.target_id;
    if (targetId == -1)
        return -1;

    for (int i = 0; i < map->entity_count; i++) {
        MapEntity *tmp = &map->entities[i];
        if (tmp->instance_id != targetId)
            continue;

        // Direct check—no guessing, no fallback logic needed!
        return (tmp->trait_flags & trait) ? i : -1;
    }

    return -1;
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

void AssignNewMovementComponent(Map *map, MapEntity *entity) {
    // Scan the fixed pool for an available inactive slot
    for (int i = 0; i < 250; i++) {
        if (!map->movement_components[i].is_active) {
            // Clear out old data from the slot
            memset(&map->movement_components[i], 0, sizeof(MovementComponent));
            map->movement_components[i].is_active = true;

            entity->movement_component_id = i;
            return;
        }
    }

    // If we get here, all 250 slots are taken!
    TraceLog(LOG_ERROR, "Movement component pool exhausted (max 250 reached)!");
    entity->movement_component_id = -1;
}

void AssignNewCombatComponent(Map *map, MapEntity *entity) {
    // Scan the fixed pool for an available inactive slot
    for (int i = 0; i < 250; i++) {
        if (!map->combat_components[i].is_active) {
            // Clear out old data from the slot
            memset(&map->combat_components[i], 0, sizeof(CombatComponent));
            map->combat_components[i].is_active = true;

            entity->combat_component_id = i;
            return;
        }
    }

    // If we get here, all 250 slots are taken!
    TraceLog(LOG_ERROR, "Combat component pool exhausted (max 250 reached)!");
    entity->combat_component_id = -1;
}

void SpawnFloatingText(Map *map, Vector2 worldPos, const char *text,
                       Color color) {
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
        if (!map->floating_text_pool[i].is_active) {
            map->floating_text_pool[i].is_active = true;
            map->floating_text_pool[i].position = worldPos;
            snprintf(map->floating_text_pool[i].text,
                     sizeof(map->floating_text_pool[i].text), "%s", text);
            map->floating_text_pool[i].color = color;
            map->floating_text_pool[i].lifetime = 1.0f; // Lasts 1 second
            map->floating_text_pool[i].max_lifetime = 1.0f;
            return;
        }
    }
}

void ApplyDamageToEnemy(Map *map, int enemy_entity_index, int damage) {
    MapEntity *target = &map->entities[enemy_entity_index];
    if (target->type != ENTITY_ENEMY)
        return;

    if (target->combat_component_id == INVALID_INDEX)
        return;
    CombatComponent *enemy_combat_component =
        &map->combat_components[target->combat_component_id];

    // Apply damage
    enemy_combat_component->hp -= damage;

    // Check for death
    if (enemy_combat_component->hp <= 0) {
        if (PLAYER->targeting.locked &&
            PLAYER->targeting.target_id == target->instance_id) {
            PLAYER->targeting.locked = false;
        }

        WeaponSlot *slot = GetActiveWeaponsSlot(&PLAYER->weapon_grid);
        if (slot) {
            Enemy *enemy = &ENEMY_REGISTRY[target->entity_id];
            int gained_exp = enemy->exp;
            slot->exp += gained_exp;

            char expStr[32];
            snprintf(expStr, sizeof(expStr), "+%d EXP", gained_exp);
            SpawnFloatingText(map, target->position, expStr, GOLD);
            while (slot->exp >= slot->next_level_exp) {
                slot->exp = slot->exp % slot->next_level_exp;
                int base_socket_count = slot->max_socket / slot->level;
                int base_level_exp = slot->next_level_exp / slot->level;
                slot->level++;
                slot->max_socket = base_socket_count * slot->level;
                slot->next_level_exp = base_level_exp * slot->level;
                char lvlStr[32];
                snprintf(lvlStr, sizeof(lvlStr), "Level up! %s +%d!", GetName(ENTITY_ITEM, slot->weapon_id), slot->level);
                SpawnFloatingText(map, map->player.position, lvlStr, COLOR_CAMO_WOODLAND_GREEN);
            }
        }

        // REMOVE THE ENEMY
        RemoveEntityAt(map, enemy_entity_index);
    }
}
