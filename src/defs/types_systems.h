#ifndef TYPES_SYSTEMS
#define TYPES_SYSTEMS
#include "raylib.h"
#include <stdint.h>
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/constants.h"

typedef struct Input{
  Vector2 dir;
  Vector2 mouse;
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
  BACKSPACE_PRESSED = 1 <<6,
  SHIFT_PRESSED = 1 << 7,
  CONTROL_PRESSED = 1 <<8,
  MOVEMENT_PRESSED = 1 <<15,
  LEFT_MOUSE_CLICKED = 1 <<16,
  LEFT_MOUSE_DOWN = 1 <<17,
  LEFT_MOUSE_RELEASED = 1 <<18,
} ButtonPressed;

typedef struct ScriptManager{
  Message* active_messsage;
  int count;
  int capacity;
  bool active;
  int currentID;
} ScriptManager;

typedef struct player{
  Inventory inventory;
  int mineral_inventory[MINERAL_COUNT];
  float speed;
  Texture2D sprite;     // How fast we move
} Player;

#endif
