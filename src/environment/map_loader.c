#include "environment/map_loader.h"

#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "raylib.h"
#include "registry/register.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void LoadMap(const char *mapName, Map *map) {
  if (map->is_ready) {
    Close_Map(map);
  }
  memset(map, 0, sizeof(Map));

  char filePath[256];
  snprintf(filePath, sizeof(filePath), "data/maps/%s.map", mapName);

  FILE *file = fopen(filePath, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filePath);
    return;
  }
  TraceLog(LOG_INFO, "File Loaded - %s", filePath);

  char line[1024];

  // 1. Read Metadata Header (rows,columns,name)
  if (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;
    char *rowToken = strtok(line, ",");
    char *colToken = strtok(NULL, ",");
    char *nameToken = strtok(NULL, ",");
    if (rowToken && colToken) {
      map->rows = atoi(rowToken);
      map->columns = atoi(colToken);
      if (nameToken) {
        strncpy(map->name, nameToken, sizeof(map->name) - 1);
        map->name[sizeof(map->name) - 1] = '\0';
      }
    }
  }

  // 2. Read Grid Types
  for (int row = 0; row < map->rows; row++) {
    if (!fgets(line, sizeof(line), file))
      break;
    line[strcspn(line, "\n")] = 0;
    char *colToken = strtok(line, ",");
    for (int col = 0; col < map->columns; col++) {
      if (colToken != NULL) {
        map->grid[row][col].type = atoi(colToken);
        colToken = strtok(NULL, ",");
      }
    }
  }

  // 3. Read Grid Heights (Fixed Row Count Loop)
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;
    if (strlen(line) == 0)
      continue; // Skip blank separator lines

    // Parse Row 0
    char *colToken = strtok(line, ",");
    for (int col = 0; col < map->columns; col++) {
      if (colToken != NULL) {
        map->grid[0][col].height = atoi(colToken);
        colToken = strtok(NULL, ",");
      }
    }

    // Parse Remaining Rows (1 to map->rows - 1)
    for (int row = 1; row < map->rows; row++) {
      if (!fgets(line, sizeof(line), file))
        break;
      line[strcspn(line, "\n")] = 0;
      colToken = strtok(line, ",");
      for (int col = 0; col < map->columns; col++) {
        if (colToken != NULL) {
          map->grid[row][col].height = atoi(colToken);
          colToken = strtok(NULL, ",");
        }
      }
    }
    break; // Heights section complete
  }

  // Compute Isometric Positions
  for (int i = 0; i < map->rows; i++) {
    for (int j = 0; j < map->columns; j++) {
      map->grid[i][j].isoPos =
          GetWorldToIso((Vector2){j * TILE_SIZE, i * TILE_SIZE});
    }
  }
  map->entitity_id=0;

  // 4. Read Entities (Header updated to check grid_x/grid_y)
  // map->entities = NULL;
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;
    if (strncmp(line, "buildingid", 10) == 0 ||
        strncmp(line, "[buildings]", 11) == 0)
      break;
    if (strlen(line) == 0 || strncmp(line, "type", 4) == 0)
      continue;

    char *typeToken = strtok(line, ",");
    char *gxToken = strtok(NULL, ",");
    char *gyToken = strtok(NULL, ",");
    char *idToken = strtok(NULL, ",");

    if (typeToken && gxToken && gyToken && idToken) {
      MapEntity *m = AddEntity(map);
      if (m == NULL)
        continue;
      m->instance_id = map->entitity_id++;
      m->type = (EntityType)atoi(typeToken);
      m->jumpoffset = 0.0f;

      // Parse grid coordinates and convert to world pixel units
      int gx = atoi(gxToken);
      int gy = atoi(gyToken);
      m->position = (Vector2){(float)(gx * TILE_SIZE), (float)(gy * TILE_SIZE)};
      m->entity_id = atoi(idToken);
      m->trait_flags = GetDefaultTraitFlags(m->type, m->entity_id);

      if (m->type == ENTITY_ENEMY) {
        m->hp = ENEMY_REGISTRY[m->entity_id].hp;
      }
      if (m->type == ENTITY_CHARACTER || m->type == ENTITY_ENEMY) {
        m->behavior = BEHAVIOR_WANDER;
        m->speed = 30.0f;
        m->target_position = m->position;
      }

      if (gx >= 0 && gx < map->columns && gy >= 0 && gy < map->rows) {
        float startFloor = map->grid[gy][gx].height * 8.0f;
        m->altitude = startFloor;
      } else {
        m->altitude = 0.0f;
      }
      m->isCollecting = false;
    }
  }
  TraceLog(LOG_INFO,"Loaded %d enities from %s.map", map->entity_count, map->name);
  // 5. Read Buildings
  map->buildings = NULL;
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;

    if (strncmp(line, "active_nodes", 12) == 0)
      break;
    if (strlen(line) == 0 || strncmp(line, "buildingid", 10) == 0)
      continue;

    char *buildingidToken = strtok(line, ",");
    char *x1Token = strtok(NULL, ",");
    char *y1Token = strtok(NULL, ",");
    char *x2Token = strtok(NULL, ",");
    char *y2Token = strtok(NULL, ",");
    char *minHeightToken = strtok(NULL, ",");
    char *maxHeightToken = strtok(NULL, ",");
    char *totalFloorsToken = strtok(NULL, ",");
    char *doorX = strtok(NULL, ",");
    char *doorY = strtok(NULL, ",");

    if (buildingidToken && x1Token && y1Token && x2Token && y2Token &&
        minHeightToken && maxHeightToken && doorX && doorY &&
        totalFloorsToken) {
      BuildingZone *b = malloc(sizeof(BuildingZone));
      if (b == NULL)
        continue;
      b->id = atoi(buildingidToken);
      b->x1 = atoi(x1Token);
      b->y1 = atoi(y1Token);
      b->x2 = atoi(x2Token);
      b->y2 = atoi(y2Token);
      b->min_height = atoi(minHeightToken);
      b->max_height = atoi(maxHeightToken);
      b->total_floors = atoi(totalFloorsToken);
      b->door_pos = (Vector2){(float)atoi(doorX), (float)atoi(doorY)};
      b->color = COLOR_SHELL_PINK;
      AddBuilding(map, b);
    }
  }

  // 6. Read Active Nodes
  map->node_count = 0;
  for (int i = 0; i < 10; i++)
    map->active_nodes[i] = -1;
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = 0;
    if (strlen(line) == 0 || strncmp(line, "active_nodes", 12) == 0)
      continue;
    int node_id = atoi(line);
    if (node_id < 0)
      continue;
    map->active_nodes[map->node_count] = node_id;
    map->node_count++;
    if (map->node_count == 10)
      break;
  }

  fclose(file);
  map->hitstop_timer = 0.0f;
  map->is_ready = true;
}

void SaveMap(Map *map) {
  char filePath[256];
  snprintf(filePath, sizeof(filePath), "data/maps/%s.map", map->name);

  FILE *file = fopen(filePath, "w");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s for saving", filePath);
    return;
  }
  TraceLog(LOG_INFO, "File Saved - %s", filePath);

  // 1. Write Metadata Header
  fprintf(file, "%d,%d,%s\n", map->rows, map->columns, map->name);

  // 2. Write Grid Types
  for (int i = 0; i < map->rows; i++) {
    for (int j = 0; j < map->columns; j++) {
      fprintf(file, "%d", map->grid[i][j].type);
      if (j < map->columns - 1)
        fprintf(file, ",");
    }
    fprintf(file, "\n");
  }

  fprintf(file, "\n"); // Spacer line

  // 3. Write Grid Heights
  for (int i = 0; i < map->rows; i++) {
    for (int j = 0; j < map->columns; j++) {
      fprintf(file, "%d", map->grid[i][j].height);
      if (j < map->columns - 1)
        fprintf(file, ",");
    }
    fprintf(file, "\n");
  }

  fprintf(file, "\n"); // Spacer line before entities

  // 4. Write Entities Header & Rows (CONVERTED TO GRID UNITS)
  // Updated header from positionx,positiony to grid_x,grid_y
  fprintf(file, "type,grid_x,grid_y,id\n");
  for(int i=0;i<map->entity_count;i++){
      MapEntity *curr = &map->entities[i];
      int gx = (int)(curr->position.x / TILE_SIZE);
      int gy = (int)(curr->position.y / TILE_SIZE);

      fprintf(file, "%d,%d,%d,%d\n", curr->type, gx, gy, curr->entity_id);
  }

  fprintf(file, "\n"); // Spacer line before buildings

  // 5. Write Buildings Header & Rows
  fprintf(file, "buildingid,x1,y1,x2,y2,min_height,max_height,total_floors,"
                "door_x,door_y\n");
  BuildingZone *b_curr = map->buildings;
  while (b_curr != NULL) {
    fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", b_curr->id, b_curr->x1,
            b_curr->y1, b_curr->x2, b_curr->y2, b_curr->min_height,
            b_curr->max_height, b_curr->total_floors, (int)b_curr->door_pos.x,
            (int)b_curr->door_pos.y);
    b_curr = b_curr->next;
  }

  fprintf(file, "\n"); // Spacer line before active nodes

  // 6. Write Active Nodes Header & List
  fprintf(file, "active_nodes\n");
  for (int i = 0; i < map->node_count; i++) {
    if (map->active_nodes[i] >= 0) {
      fprintf(file, "%d\n", map->active_nodes[i]);
    }
  }

  fclose(file);
}
