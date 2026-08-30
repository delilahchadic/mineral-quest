#ifndef TYPES_ENV
#define TYPES_ENV
#include <stdint.h>
#include "raylib.h"
#include "defs/types_entities.h"

#define MAP_WIDTH 256
#define MAP_HEIGHT 256
// Physical dimensions in pixels
#define WORLD_WIDTH_PX  (MAP_WIDTH * TILE_SIZE)
#define WORLD_HEIGHT_PX (MAP_HEIGHT * TILE_SIZE)

typedef enum{MAP_ITEM,MAP_NPC,MAP_OBJECT,MAP_TRANSITION}MapEnityType;

typedef enum State{
  NORMAL_STATE,
  JUMPING_STATE
} State;

typedef enum Behavior{
    IMMOBILE,
    WANDER
} Behavior;

typedef enum ComboState{
    COMBO_NONE,
    COMBO_1,
    COMBO_2,
    COMBO_3
} ComboState;

typedef struct {
    bool isAttacking;
    float attackTimer;
    float attackDuration;
    float attackAngle;
    ComboState combo_state;
    float combo_timer;
    float facing_direction; // Base angle (e.g. 0 for right, PI for left)
} CombatState;

typedef struct MapEntity {
  EntityType type;      // Is this a person or a flower?
  Vector2 position; // Pointer to the actual Character, Item, or Plant struct
  Vector2 target_position;
  Behavior behavior;
  struct MapEntity* next;
  int id;
  State state;
  Vector2 velocity;
  float speed;
  float jumpoffset;
  float vertical_velocity;
  float altitude;
  uint32_t trait_flags;
  struct MapEntity* next_in_bucket;
  CombatState combat;
  bool hitThisSwing;
} MapEntity;

typedef struct Tile{
  int height;
  TileType type;
  Vector2 isoPos;
} Tile;

typedef struct Map{
  Tile grid[MAP_WIDTH][MAP_HEIGHT];
  int pixel_width;
  int pixel_height;
  int rows;
  int columns;
  char name[32];
  MapEntity* entities;
  MapEntity* player;
  int lastTileHeight;
  bool is_ready;
  MapEntity* buckets[MAP_HEIGHT];
}Map;
#endif
