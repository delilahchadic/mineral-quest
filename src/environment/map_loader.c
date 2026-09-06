#include "environment/map_loader.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "environment/map.h"
#include "raylib.h"
#include "registry/register.h"

void LoadMap(const char* mapName, Map* map) {
    memset(map, 0, sizeof(Map));

    char filePath[256];
    snprintf(filePath, sizeof(filePath), "data/maps/%s.map", mapName);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "Failed to open %s", filePath);
        return;
    }
    TraceLog(LOG_INFO, "File Loaded - %s", filePath);

    char line[1024];

    // 1. Read Metadata Header (rows,columns,name)
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char* rowToken = strtok(line, ",");
        char* colToken = strtok(NULL, ",");
        char* nameToken = strtok(NULL, ",");
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
        if (!fgets(line, sizeof(line), file)) break;
        line[strcspn(line, "\n")] = 0;
        char* colToken = strtok(line, ",");
        for (int col = 0; col < map->columns; col++) {
            if (colToken != NULL) {
                map->grid[row][col].type = atoi(colToken);
                colToken = strtok(NULL, ",");
            }
        }
    }

    // 3. Read Grid Heights (skip any potential blank separator lines if present)
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            // If it's not empty, assume it's the start of grid heights
            // Put it back or handle row 0 right here:
            char* colToken = strtok(line, ",");
            for (int col = 0; col < map->columns; col++) {
                if (colToken != NULL) {
                    map->grid[0][col].height = atoi(colToken);
                    colToken = strtok(NULL, ",");
                }
            }
            // Read remaining rows for heights
            for (int row = 1; row < map->rows; row++) {
                if (!fgets(line, sizeof(line), file)) break;
                line[strcspn(line, "\n")] = 0;
                colToken = strtok(line, ",");
                for (int col = 0; col < map->columns; col++) {
                    if (colToken != NULL) {
                        map->grid[row][col].height = atoi(colToken);
                        colToken = strtok(NULL, ",");
                    }
                }
            }
            break;
        }
    }

    // Compute Isometric Positions
    for (int i = 0; i < map->rows; i++) {
        for (int j = 0; j < map->columns; j++) {
            map->grid[i][j].isoPos = GetWorldToIso((Vector2){ j * TILE_SIZE, i * TILE_SIZE });
        }
    }

    // 4. Read Entities (look for section header or read until EOF)
    map->entities = NULL;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strncmp(line, "buildingid", 10) == 0 || strncmp(line, "[buildings]", 11) == 0) {
                    // Put it back or let the next loop handle building data
                    // Since we already read it, we need to handle it or jump to building parsing.
                    // A clean way is to let the loop drop through to the building parser:
                    break;
                }
        // Skip empty lines or header labels if you include them (like "type,positionx...")
        if (strlen(line) == 0 || strncmp(line, "type", 4) == 0) continue;

        char* typeToken = strtok(line, ",");
        char* xToken = strtok(NULL, ",");
        char* yToken = strtok(NULL, ",");
        char* idToken = strtok(NULL, ",");

        if (typeToken && xToken && yToken && idToken) {
            MapEntity* m = malloc(sizeof(MapEntity));
            if (m == NULL) continue;

            m->type = (EntityType)atoi(typeToken);
            m->jumpoffset = 0.0f;
            m->position = (Vector2){atof(xToken), atof(yToken)};
            m->next = NULL;
            m->id = atoi(idToken);
            m->trait_flags = GetDefaultTraitFlags(m->type, m->id);

            if (m->type == ENTITY_ENEMY) {
                m->hp = ENEMY_REGISTRY[m->id].hp;
            }
            if (m->type == ENTITY_CHARACTER || m->type == ENTITY_ENEMY) {
                m->behavior = WANDER;
                m->speed = 30.0f;
                m->target_position = m->position;
            }

            int tx = (int)(m->position.x / TILE_SIZE);
            int ty = (int)(m->position.y / TILE_SIZE);

            if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
                float startFloor = map->grid[ty][tx].height * 8.0f;
                m->altitude = startFloor;
            } else {
                m->altitude = 0.0f;
            }

            Add_Entity(map, m);
        }
    }

    map->buildings =NULL;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines or header labels if you include them (like "type,positionx...")
        if (strlen(line) == 0 || strncmp(line, "buildingid", 10) == 0) continue;
        // buildingid,x1, y1,x2, y2,min_height,max_height,total_floors,door_x,door_y
        char* buildingidToken = strtok(line, ",");
        char* x1Token = strtok(NULL, ",");
        char* y1Token = strtok(NULL, ",");
        char* x2Token = strtok(NULL, ",");
        char* y2Token = strtok(NULL, ",");
        char* minHeightToken = strtok(NULL, ",");
        char* maxHeightToken = strtok(NULL, ",");
        char* totalFloorsToken = strtok(NULL, ",");
        char* doorX = strtok(NULL, ",");
        char* doorY = strtok(NULL, ",");
        if(buildingidToken && x1Token && y1Token && x2Token &&
            y2Token && minHeightToken && maxHeightToken && doorX && doorY && totalFloorsToken){
                BuildingZone* b = malloc(sizeof(BuildingZone));
                if(b==NULL) continue;
                b->id =atoi(buildingidToken);
                b->x1 =atoi(x1Token);
                b->y1 =atoi(y1Token);
                b->x2 =atoi(x2Token);
                b->y2 =atoi(y2Token);
                b->min_height=atoi(minHeightToken);
                b->max_height=atoi(maxHeightToken);
                b->total_floors = atoi(totalFloorsToken);
                b->door_pos = (Vector2){atoi(doorX),atoi(doorY)};

                b->next=map->buildings;
                map->buildings=b;
            }
    }
    fclose(file);
    map->hitstop_timer = 0.0f;
    map->is_ready = true;
}

void SaveMap(Map* map) {
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "data/maps/%s.map", map->name);

    FILE* file = fopen(filePath, "w");
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
            if (j < map->columns - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\n"); // Spacer line

    // 3. Write Grid Heights
    for (int i = 0; i < map->rows; i++) {
        for (int j = 0; j < map->columns; j++) {
            fprintf(file, "%d", map->grid[i][j].height);
            if (j < map->columns - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }

    fprintf(file, "\n"); // Spacer line before entities

    // 4. Write Entities Header & Rows
    fprintf(file, "type,positionx,positiony,id\n");
    MapEntity* curr = map->entities;
    while (curr != NULL) {
        fprintf(file, "%d,%f,%f,%d\n", curr->type, curr->position.x, curr->position.y, curr->id);
        curr = curr->next;
    }

    fprintf(file, "\n"); // Spacer line before buildings

        // 5. Write Buildings Header & Rows
        fprintf(file, "buildingid,x1,y1,x2,y2,min_height,max_height,total_floors,door_x,door_y\n");
        BuildingZone* b_curr = map->buildings;
        while (b_curr != NULL) {
            fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                b_curr->id,
                b_curr->x1, b_curr->y1,
                b_curr->x2, b_curr->y2,
                b_curr->min_height, b_curr->max_height,
                b_curr->total_floors,
                (int)b_curr->door_pos.x, (int)b_curr->door_pos.y
            );
            b_curr = b_curr->next;
        }
    fclose(file);
}
