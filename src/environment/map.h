#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include "defs/types_env.h"
#include "defs/constants.h"

#define TILE_SIZE 32
#define MAP_WIDTH 256
#define MAP_HEIGHT 256
// Physical dimensions in pixels
#define WORLD_WIDTH_PX  (MAP_WIDTH * TILE_SIZE)
#define WORLD_HEIGHT_PX (MAP_HEIGHT * TILE_SIZE)

void InitNewMap(Map* map,char* name,int rows, int columns);
void InitMap(Map* map);
void Draw_MapEntity(MapEntity* entity, Map* map);
void UpdateEntityMovement(Map* map, float dt);
void Draw_Map(Map* map, Camera2D* camera);
void DrawWaterEffects(Map* map,int x, int y);
void DrawSimpleSparkle(Vector2 pos, Color color, float size);
void ResetAllHitFlags(Map* map);
void Close_Map(Map* map);
void Add_Entity(Map* map, MapEntity* entity);
void Remove_Entity(Map* map, MapEntity* entity);
void Init_Player(Map* map);
void Draw_Tile(Map* map, int x, int y);
Vector2 GetWorldToIso(Vector2 worldPos);
Vector2 GetIsoWorldToGrid(Vector2 worldPos);
Vector2 GetIsoWorldToGridWithHeight(Map* map, Vector2 screenWorldPos);
Vector2 GetGridToIsoWorld(int x, int y);
MapEntity*  PollTrait(Map* map, TraitFlags traits, float distance);
void AddBuilding(Map* map, BuildingZone* building);
void Remove_Building(Map* map, BuildingZone* building);

#endif
