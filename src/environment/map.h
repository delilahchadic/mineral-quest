#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"
#define TILE_SIZE 32
#include "raylib.h"
#define MAP_WIDTH 100
#define MAP_HEIGHT 100
// Physical dimensions in pixels
#define WORLD_WIDTH_PX  (MAP_WIDTH * TILE_SIZE)
#define WORLD_HEIGHT_PX (MAP_HEIGHT * TILE_SIZE)

typedef enum { TILE_WATER, TILE_STONE,TILE_SAND, TILE_DIRT,TILE_GRASS, TILE_ROAD} TileType;
typedef enum{MAP_ITEM,MAP_NPC,MAP_OBJECT,MAP_TRANSITION}MapEnityType;


typedef struct TileDefinition{
    int id;             // 0 = Sand, 1 = Asphalt, 2 = Magnetic Pit
    bool is_blocking;   //
    float friction;     // 1.0 = Normal, 0.2 = Ice/Oil, 1.5 = Deep Sand
    int footstep_sfx;
    Color color;// sound??
} TileDefinition;

typedef enum {
    ENTITY_CHARACTER,
    ENTITY_ITEM,
    ENTITY_PLANT,
    ENTITY_DECOR
} EntityType;

typedef struct MapEntity {
    EntityType type;      // Is this a person or a flower?
    Vector2 position;     // Physical location on the highway
    char name[32];      // Pointer to the actual Character, Item, or Plant struct
    struct MapEntity* next; // For the linked list
} MapEntity;

typedef struct Map{
  int grid[MAP_WIDTH][MAP_HEIGHT];
  int pixel_width;
  int pixel_height;
  int rows;
  int columns;
  char name[32];
  MapEntity* entities;
}Map;

void LoadMapGridFile(const char* filename, Map* map);
void InitMap(Map* map);
void Draw_Map(Map* map);
bool Check_Collision(Map* map, Vector2 nextPos);
void Add_Entity(Map* map, MapEntity* entity);



#endif