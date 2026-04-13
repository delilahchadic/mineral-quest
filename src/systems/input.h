#ifndef INPUT_H
#define INPUT_H
#include "raylib.h"

typedef struct Input{
  Vector2 dir;
  bool jump;
  bool inventory;
  bool dialog;
} Input;

Input CaptureInput();
#endif