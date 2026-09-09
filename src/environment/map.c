#include "map.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "defs/types_env.h"
#include "engine/palette.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "systems/input.h"
#include "registry/register.h"
#include "raymath.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "raylib.h"
#include "registry/mineral_register.h"

typedef enum ElementType{
    ELEMENT_NONE,
    ELEMENT_FIRE,
    ELEMENT_WATER
}ElementType;

void InitMap(Map* map){
  Init_Player(map);
  map->lastTileHeight = -1;
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
  map->is_ready = true;
}

void InitNewMap(Map* map,char* name,int columns, int rows){
  memset(map, 0, sizeof(Map));
  snprintf(map->name, sizeof(map->name),"%s", name);
  map->name[sizeof(map->name) - 1] = '\0';
  map->rows = rows;
  map->columns = columns;
  for(int i =0;i<map->rows;i++){
    for(int j =0;j<map->columns;j++){
      map->grid[i][j].height = 0;
      map->grid[i][j].type = TILE_GRASS;
      map->grid[i][j].isoPos = GetWorldToIso((Vector2){ j * TILE_SIZE, i * TILE_SIZE });
    }
  }

  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
  map->is_ready = true;
}

void Init_Player(Map* map){
  MapEntity* player = malloc(sizeof(MapEntity));
  player->type = ENTITY_PLAYER;
  player->state = NORMAL_STATE;
  player->position = (Vector2){30,30};
  Add_Entity(map,player);
  map->player = player;
  player->jumpoffset = 0.0f;

  player->combat.isAttacking = false;
  player->combat.attackTimer = 0;
  player->combat.attackDuration = 0.25f;
  player->stats = GetStats(ENTITY_PLAYER, 0);
}

void Close_Map(Map* map){
  if(map == NULL){
    return;
  }

  MapEntity* e = NULL;
  e = map->entities;
  while(e != NULL){
    map->entities = e->next;
    free(e);
    e = map->entities;
  }

  BuildingZone* b = NULL;
  b = map->buildings;
  while(b != NULL){
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
    return (Vector2){ worldX, worldY };
}

Vector2 GetIsoWorldToGridWithHeight(Map* map, Vector2 screenWorldPos) {
    // 1. Define your max height (matches your grid limits)
    // If your max height is 10, start there.
    const int MAX_HEIGHT = 150;
    const float HEIGHT_STEP = 8.0f; // From your Draw_Tile: height * 8.0f

    for (int h = MAX_HEIGHT; h >= 0; h--) {
        // 2. Offset the Y coordinate to 'drop' the screen click
        // to the level of the current height slice.
        float pixelOffset = h * HEIGHT_STEP;
        Vector2 testPos = { screenWorldPos.x, screenWorldPos.y + pixelOffset };

        // 3. Use your existing math to find what grid cell that corresponds to
        Vector2 grid = GetIsoWorldToGrid(testPos);

        int ix = (int)grid.x;
        int iy = (int)grid.y;

        // 4. Validate the grid index
        if (ix >= 0 && ix < map->columns && iy >= 0 && iy < map->rows) {
            // 5. Check: Is the tile at this grid coordinate actually at this height?
            // We check >= because you might be clicking the "side" of a tall block.
            if (map->grid[iy][ix].height >= h) {
                return (Vector2){ (float)ix, (float)iy };
            }
        }
    }

    // Fallback: If nothing was hit, return the flat ground (height 0)
    return GetIsoWorldToGrid(screenWorldPos);
}

void ResetAllHitFlags(Map* map) {
    MapEntity* e = map->entities;
    while (e != NULL) {
        e->hitThisSwing = false;
        e = e->next;
    }
}

void Remove_Entity(Map* map, MapEntity* entity){
  if(map==NULL || entity == NULL) return;

  if(map->entities == entity){
    map->entities = entity->next;
    entity->next =  NULL;
    return;
  }

  MapEntity* curr = map->entities;
  while(curr->next != NULL && curr->next != entity){
    curr = curr->next;
  }
  if(curr->next == entity){
    curr->next = entity->next;
    entity->next = NULL;
    if(entity->type != ENTITY_PLAYER) free(entity);
    return;
  }
}

void AddBuilding(Map* map, BuildingZone* building){
    if(building == NULL) return;
    building->next=map->buildings;
    map->buildings=building;
    map->building_id++;
}
void Remove_Building(Map* map, BuildingZone* building){
  if(map==NULL || building == NULL) return;

  if(map->buildings == building){
    map->buildings = building->next;
    building->next =  NULL;
    return;
  }

  BuildingZone* curr = map->buildings;
  while(curr->next != NULL && curr->next != building){
    curr = curr->next;
  }
  if(curr->next == building){
    curr->next = building->next;
    building->next = NULL;
    free(building);
    return;
  }
}

void Add_Entity(Map* map, MapEntity* entity){
  entity->next = map->entities;
  map->entities = entity;
  return;
}

MapEntity* PollTrait(Map* map, TraitFlags trait, float distance){
  MapEntity* tmp = map->entities;
  while (tmp != NULL) {
    if(tmp->trait_flags & trait){
      if(Vector2Distance(map->player->position, tmp->position) < distance){
        return tmp;
      }
    }
    tmp = tmp->next;
  }
  return NULL;
}

bool PickNewWanderTarget(Map* map, MapEntity* entity) {
    int cur_tx = (int)(entity->position.x / TILE_SIZE);
    int cur_ty = (int)(entity->position.y / TILE_SIZE);

    if (cur_tx < 0 || cur_tx >= map->columns || cur_ty < 0 || cur_ty >= map->rows) return false;
    int cur_h = map->grid[cur_ty][cur_tx].height;

    for (int i = 0; i < 10; i++) {
        // Generate candidate around current position
        float cx = entity->position.x + ((rand() % 128) - 64);
        float cy = entity->position.y + ((rand() % 128) - 64);

        // Clamp to map boundaries so it never goes into negative space or off-screen
        if (cx < 0.0f) cx = 0.0f;
        if (cx > (float)map->pixel_width) cx = (float)map->pixel_width;
        if (cy < 0.0f) cy = 0.0f;
        if (cy > (float)map->pixel_height) cy = (float)map->pixel_height;

        int t_tx = (int)(cx / TILE_SIZE);
        int t_ty = (int)(cy / TILE_SIZE);

        if (t_tx >= map->columns) t_tx = map->columns - 1;
        if (t_ty >= map->rows) t_ty = map->rows - 1;

        if (t_tx >= 0 && t_tx < map->columns && t_ty >= 0 && t_ty < map->rows) {
            if (abs(map->grid[t_ty][t_tx].height - cur_h) <= 2) {
                entity->target_position = (Vector2){cx, cy};
                return true;
            }
        }
    }
    return false;
}

void UpdateEntityMovement(Map* map, float dt) {
    MapEntity* entity = map->entities;
    while (entity) {
        if (entity->behavior == WANDER) {
            Vector2 diff = Vector2Subtract(entity->target_position, entity->position);
            float distance = Vector2Length(diff);

            if (distance > 1.0f) {
                entity->position = Vector2Add(entity->position, Vector2Scale(Vector2Normalize(diff), entity->speed * dt));

                // Hard clamp position to stay within map pixel boundaries
                if (entity->position.x < 0.0f) entity->position.x = 0.0f;
                if (entity->position.x > (float)map->pixel_width) entity->position.x = (float)map->pixel_width;
                if (entity->position.y < 0.0f) entity->position.y = 0.0f;
                if (entity->position.y > (float)map->pixel_height) entity->position.y = (float)map->pixel_height;

                int tx = (int)(entity->position.x / TILE_SIZE);
                int ty = (int)(entity->position.y / TILE_SIZE);

                // Prevent array out-of-bounds if exactly on the right/bottom edge
                if (tx >= map->columns) tx = map->columns - 1;
                if (ty >= map->rows) ty = map->rows - 1;

                if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
                    float targetAlt = map->grid[ty][tx].height * 8.0f;
                    entity->altitude += (targetAlt - entity->altitude) * 10.0f * dt;
                }
            } else if (!PickNewWanderTarget(map, entity)) {
                entity->target_position = entity->position;
            }
        }
        entity = entity->next;
    }
}
