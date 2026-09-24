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

  if(IsKeyPressed(KEY_B)){
    i.buttons_pressed |= KEY_B_PRESSED;
  }

  if(IsKeyPressed(KEY_Z)){
    i.buttons_pressed |= KEY_Z_PRESSED;
  }
  if(IsKeyPressed(KEY_X)){
    i.buttons_pressed |= KEY_X_PRESSED;
  }
  if(IsKeyPressed(KEY_C)){
    i.buttons_pressed |= KEY_C_PRESSED;
  }
  if(IsKeyPressed(KEY_S)){
    i.buttons_pressed |= KEY_S_PRESSED;
  }

  if(IsKeyPressed(KEY_L)){
    i.buttons_pressed |= KEY_L_PRESSED;
  }

  if(IsKeyPressed(KEY_I)){
    i.buttons_pressed |= KEY_I_PRESSED;
  }
  if(IsKeyPressed(KEY_J)){
    i.buttons_pressed |= KEY_J_PRESSED;
  }
  if(IsKeyPressed(KEY_G)){
    i.buttons_pressed |= KEY_G_PRESSED;
  }
  if(IsKeyPressed(KEY_K)){
    i.buttons_pressed |= KEY_K_PRESSED;
  }

  if(IsKeyPressed(KEY_O)){
    i.buttons_pressed |= KEY_O_PRESSED;
  }
  if(IsKeyPressed(KEY_U)){
    i.buttons_pressed |= KEY_U_PRESSED;
  }

  if(IsKeyPressed(KEY_P)){
    i.buttons_pressed |= KEY_P_PRESSED;
  }

  if(IsKeyPressed(KEY_E)){
    i.buttons_pressed |= KEY_E_PRESSED;
  }
  if(IsKeyDown(KEY_SPACE)){
    i.buttons_pressed |= JUMP_PRESSED;
  }
  if(IsKeyPressed(KEY_M)){
    i.buttons_pressed |= KEY_M_PRESSED;
  }

  if(IsKeyPressed(KEY_N)){
    i.buttons_pressed |= KEY_N_PRESSED;
  }

  if(IsKeyDown(KEY_LEFT_SHIFT)){
    i.buttons_pressed |= SHIFT_DOWN;
  }

  if(IsKeyPressed(KEY_LEFT_SHIFT)){
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

  if (i.buttons_pressed & KEY_I_PRESSED) i.attack_dir.y -= 1.0f; // Up
  if (i.buttons_pressed & KEY_K_PRESSED) i.attack_dir.y += 1.0f; // Down
  if (i.buttons_pressed & KEY_J_PRESSED) i.attack_dir.x -= 1.0f; // Left
  if (i.buttons_pressed & KEY_L_PRESSED) i.attack_dir.x += 1.0f; // Right
  return i;
}
