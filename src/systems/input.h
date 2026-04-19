#ifndef INPUT_H
#define INPUT_H
#include "raylib.h"
#include <stdint.h>
typedef struct Input{
  Vector2 dir;
  uint32_t buttons_pressed;
} Input;

typedef enum ButtonPressed{
  NONE_PRESSED = 0,
  INVENTORY_PRESSED = 1 << 0,
  INTERACT_PRESSED = 1 << 1,
  JUMP_PRESSED = 1 << 2,
  KEY_W_PRESSED = 1 <<3,
  KEY_S_PRESSED = 1 <<4,
  MOVEMENT_PRESSED = 1 <<7
} ButtonPressed;

Input CaptureInput();

#endif