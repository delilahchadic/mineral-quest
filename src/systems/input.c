#include "systems/input.h"

Input CaptureInput(){
  Input i = {0};
  
  if (IsKeyDown(KEY_W)) i.dir.y -= 1;
  if (IsKeyDown(KEY_A)) i.dir.x -= 1;
  if (IsKeyDown(KEY_D)) i.dir.x += 1;
  if (IsKeyDown(KEY_S)) i.dir.y += 1;

  if(IsKeyPressed(KEY_I)){
    i.buttons_pressed |= INVENTORY_PRESSED;
  }
  if(IsKeyPressed(KEY_E)){
    i.buttons_pressed |= INTERACT_PRESSED;
  } 
  if(IsKeyDown(KEY_SPACE)){
    i.buttons_pressed |= JUMP_PRESSED;
  }
  
  if(i.dir.x != 0 || i.dir.y != 0){
    i.buttons_pressed |= MOVEMENT_PRESSED;
  }

  return i;
}