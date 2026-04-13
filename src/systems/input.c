#include "systems/input.h"

Input CaptureInput(){
  Input i = {0};
  
  if (IsKeyDown(KEY_W)) i.dir.y -= 1;
  if (IsKeyDown(KEY_A)) i.dir.x -= 1;
  if (IsKeyDown(KEY_D)) i.dir.x += 1;
  if (IsKeyDown(KEY_S)) i.dir.y += 1;
  i.jump = IsKeyDown(KEY_SPACE);
  i.inventory = IsKeyDown(KEY_I);
  i.dialog = IsKeyDown(KEY_E);

  return i;
}