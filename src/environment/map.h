#ifndef MAP_H
#define MAP_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "raylib.h"
#include "raymath.h"
#include "engine/palette.h"
#include "systems/character.h"
#include "systems/player.h"
#include "systems/dialog.h"
#include "engine/register.h"
#define TILE_SIZE 32

#define MAP_WIDTH 100
#define MAP_HEIGHT 100
// Physical dimensions in pixels
#define WORLD_WIDTH_PX  (MAP_WIDTH * TILE_SIZE)
#define WORLD_HEIGHT_PX (MAP_HEIGHT * TILE_SIZE)

typedef enum { TILE_WATER,TILE_GRASS ,TILE_SAND, TILE_DIRT,TILE_STONE, TILE_ROAD} TileType;
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
    ENTITY_DECOR,
    ENTITY_PLAYER
} EntityType;

typedef enum {
  NORMAL_STATE,
  JUMPING_STATE
} State;

typedef struct MapEntity {
    // bool isPlayer;
    EntityType type;      // Is this a person or a flower?
    Vector2 position;     // Physical location on the highway
    char name[32];      // Pointer to the actual Character, Item, or Plant struct
    struct MapEntity* next; 
    Texture2D* sprite; // For the linked list
    union{
      Character* character;
      Plant* plant;
      Player player;
      int id;
    } data;
    State state;
    float jumpoffset;
    float verticalVelocity;
} MapEntity;

typedef struct Tile{
  int height;
  TileType type;
} Tile;

typedef struct Map{
  Tile grid[MAP_WIDTH][MAP_HEIGHT];
  int pixel_width;
  int pixel_height;
  int rows;
  int columns;
  char name[32];
  Camera2D camera;
  MapEntity* entities;
  MapEntity* player; 
  int lastTileHeight;
}Map;



void LoadMapGridFile(const char* filename, Map* map);
void InitMap(Map* map);
void Draw_Tiles(Map* map);
void Draw_MapEntity(MapEntity* entity, Map* map);
void Draw_Map(Map* map);
void Apply_Gravity(Map* map);
bool Check_Collision(Map* map, Vector2 nextPos);
void Add_Entity(Map* map, MapEntity* entity);
void Handle_Input(Map* map);
void Update_Map(Map* map,Dialog_Manager* manager);
void Close_Map(Map* map);
void Remove_Entity(Map* map, MapEntity* entity);
void Init_Player(Map* map); 
void AdjustCamera(Map* map,bool dialog);
void Draw_Tile(Map* map, int x, int y);
Vector2 GetWorldToIso(Vector2 worldPos);

#endif