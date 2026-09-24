#ifndef TYPES_SYSTEMS
#define TYPES_SYSTEMS
#include "raylib.h"
#include <stdint.h>
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/constants.h"

typedef struct Input{
  Vector2 dir;
  Vector2 attack_dir;
  Vector2 mouse;
  uint32_t buttons_pressed;
} Input;

typedef enum ButtonPressed{
  NONE_PRESSED = 0,
  KEY_U_PRESSED = 1 << 0,
  KEY_E_PRESSED = 1 << 1,
  JUMP_PRESSED = 1 << 2,
  KEY_W_PRESSED = 1 <<3,
  KEY_S_PRESSED = 1 <<4,
  ENTER_PRESSED = 1<<5,
  BACKSPACE_PRESSED = 1 <<6,
  SHIFT_DOWN = 1 << 7, //used for edit
  CONTROL_PRESSED = 1 <<8,
  KEY_M_PRESSED = 1 << 9,
  KEY_N_PRESSED = 1 << 10,
  KEY_P_PRESSED = 1 <<11,
  SHIFT_PRESSED = 1 <<12, // for use in play for press
  KEY_L_PRESSED = 1<<13,
  KEY_I_PRESSED = 1<<14,
  MOVEMENT_PRESSED = 1 <<15,
  LEFT_MOUSE_CLICKED = 1 <<16,
  LEFT_MOUSE_DOWN = 1 <<17,
  LEFT_MOUSE_RELEASED = 1 <<18,
  KEY_K_PRESSED = 1<<19,
  KEY_J_PRESSED = 1<<20,
  KEY_O_PRESSED = 1<<21,
  KEY_Z_PRESSED = 1<<22,
  KEY_X_PRESSED = 1<<23,
  KEY_C_PRESSED = 1<<24,
  KEY_B_PRESSED = 1 << 25,
  KEY_G_PRESSED = 1 <<26,
} ButtonPressed;

typedef struct ScriptManager{
  Message* active_messsage;
  int count;
  int capacity;
  bool active;
  int currentID;
} ScriptManager;

#endif
