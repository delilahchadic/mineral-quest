#include "play/play_inventory.h"
#include "engine/palette.h"
#include "registry/mineral_register.h"
#include <stdio.h>

void DrawInventory(Menu* menu){
  DrawMenu(menu);
}

void DrawMineralInventory(Player* player){
  // 1. Background - The Aged Paper
  ClearBackground(COLOR_PULP_PAPER);

  // Dynamic horizontal line based on screen width
  int margin = 50;
  int uiWidth = SCREEN_WIDTH - (margin * 2);

  // 2. Title - The "Ink" look
  DrawText("GEOLOGY LOG", margin, 40, 30, COLOR_SUNKEN_INK);
  DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line

  for(int i = 0;i< MINERAL_COUNT;i++){
      Color textColor = GetMineralColor(i);
      char count[5];
      sprintf(count, "%d", player->mineral_inventory[i]);
      DrawMineral(i, (Vector2){50, 120 + (i * 30) + 12});
      DrawText(GetMineralLabel(i), 100, 120 + (i * 30), 20, textColor);
      DrawText(count,250, 120 + (i * 30), 20, textColor);
  }
}

void UpdateInventory(PlaySession* session, Input* input){
  if(!UpdateMenu(&session->menu, input)){
    session->state = ADVENTURE;
  }
}

void UpdateMineralInventory(PlaySession* session, Input* input){
  if(input->buttons_pressed & MINERAL_PRESSED){
    session->state = ADVENTURE;
  }
}
