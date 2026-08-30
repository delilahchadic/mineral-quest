#include "systems/input.h"
#include "defs/types_systems.h"
#include "raylib.h"

Input CaptureInput(){
  Input i = {0};
  i.mouse = GetMousePosition();
  if (IsKeyDown(KEY_W)) i.dir.y -= 1;
  if (IsKeyDown(KEY_A)) i.dir.x -= 1;
  if (IsKeyDown(KEY_D)) i.dir.x += 1;
  if (IsKeyDown(KEY_S)) i.dir.y += 1;

  if(IsKeyPressed(KEY_W)){
    i.buttons_pressed |= KEY_W_PRESSED;
  }

  if(IsKeyPressed(KEY_S)){
    i.buttons_pressed |= KEY_S_PRESSED;
  }

  if(IsKeyPressed(KEY_I)){
    i.buttons_pressed |= INVENTORY_PRESSED;
  }
  if(IsKeyPressed(KEY_E)){
    i.buttons_pressed |= INTERACT_PRESSED;
  }
  if(IsKeyDown(KEY_SPACE)){
    i.buttons_pressed |= JUMP_PRESSED;
  }
  if(IsKeyPressed(KEY_M)){
    i.buttons_pressed |= MINERAL_PRESSED;
  }

  if(IsKeyPressed(KEY_L)){
    i.buttons_pressed |= LEVEL_PRESSED;
  }

  if(IsKeyDown(KEY_LEFT_SHIFT)){
    i.buttons_pressed |= SHIFT_PRESSED;
  }

  if(IsKeyPressed(KEY_ENTER)){
    i.buttons_pressed |= ENTER_PRESSED;
  }

  if(IsKeyDown(KEY_LEFT_SUPER)){
      i.buttons_pressed |= CONTROL_PRESSED;
  }

  if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
      i.buttons_pressed |= LEFT_MOUSE_CLICKED;
  }

  if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
      i.buttons_pressed |= LEFT_MOUSE_DOWN;

  }

  if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)){
      i.buttons_pressed |= LEFT_MOUSE_RELEASED;

  }
  if(IsKeyPressed(KEY_BACKSPACE)){
    i.buttons_pressed |= BACKSPACE_PRESSED;
  }

  if(i.dir.x != 0 || i.dir.y != 0){
    i.buttons_pressed |= MOVEMENT_PRESSED;
  }

  return i;
}
