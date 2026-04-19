#include "ui/dialog_box.h"
void DrawDialog(char* name, char* text){
  // 1. Draw the Main Box (Bottom of screen)
  int boxY = GetScreenHeight() - 150;
  DrawRectangle(50, boxY, GetScreenWidth() - 100, 120, Fade(COLOR_PULP_PAPER, 0.8f));
  DrawRectangleLines(50, boxY, GetScreenWidth() - 100, 120, COLOR_SUNKEN_INK);

  // 2. Draw the Character Name (slightly offset)
  DrawText(name, 70, boxY + 10, 20, COLOR_SUNKEN_INK);

  // 3. Draw the Dialog Text
  DrawText(text, 70, boxY + 40, 18, COLOR_SUNKEN_INK);
    
  // 4. Prompt to continue
  DrawText("Press [e] to continue", GetScreenWidth() - 250, boxY + 90, 10, GRAY);
}