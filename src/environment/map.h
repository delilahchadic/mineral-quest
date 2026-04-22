#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#include "defs/types_env.h"
#include "engine/palette.h"
#include "systems/player.h"
#include "systems/script_manager.h"
#include "systems/input.h"
#include "registry/register.h"
#define TILE_SIZE 32

#define MAP_WIDTH 256
#define MAP_HEIGHT 256

// Physical dimensions in pixels
#define WORLD_WIDTH_PX  (MAP_WIDTH * TILE_SIZE)
#define WORLD_HEIGHT_PX (MAP_HEIGHT * TILE_SIZE)

void InitNewMap(Map* map,char* name,int rows, int columns);
void InitMap(Map* map);
void Update_Map(Map* map, bool moved);
void Draw_MapEntity(MapEntity* entity, Map* map);
void Draw_Map(Map* map);


void DrawWaterTile(Vector2 t1,Vector2 t2,Vector2 t3,Vector2 t4,int x, int y);
void DrawSimpleSparkle(Vector2 pos, Color color, float size);

void Close_Map(Map* map);
void Add_Entity(Map* map, MapEntity* entity);
void Remove_Entity(Map* map, MapEntity* entity);
void Init_Player(Map* map); 

void AdjustCamera(Map* map,bool dialog);
void Draw_Tile(Map* map, int x, int y);
Vector2 GetWorldToIso(Vector2 worldPos);
MapEntity*  PollTrait(Map* map, TraitFlags traits, float distance);

#endif