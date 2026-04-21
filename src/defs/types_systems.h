#ifndef TYPES_SYSTEMS
#define TYPES_SYSTEMS
#include "raylib.h"
#include <stdint.h>
#include "defs/types_entities.h"
#define MAX_SLOTS 20
typedef struct Input{
  Vector2 dir;
  uint32_t buttons_pressed;
} Input;

typedef struct Inventory{
  int itemIds[MAX_SLOTS];
  int count;
} Inventory;

typedef enum ButtonPressed{
  NONE_PRESSED = 0,
  INVENTORY_PRESSED = 1 << 0,
  INTERACT_PRESSED = 1 << 1,
  JUMP_PRESSED = 1 << 2,
  KEY_W_PRESSED = 1 <<3,
  KEY_S_PRESSED = 1 <<4,
  ENTER_PRESSED = 1<<5,
  MOVEMENT_PRESSED = 1 <<7
} ButtonPressed;

typedef struct ScriptManager{
  Message* active_messsage;
  int count;
  int capacity;
  bool active;
  int currentID;
} ScriptManager;

typedef struct player
{
  /* data */
  Inventory inventory;
  float speed;   
  Texture2D sprite;     // How fast we move
} Player;

#endif