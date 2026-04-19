#include "ui/menu.h"

void FillMenu(Menu* menu, int(*itemIds)[MAX_SLOTS], int count){
  memcpy(&menu->itemIds, itemIds, sizeof(int) * MAX_SLOTS);
  menu->count = count;
  menu->selected = count > 0 ? 0 : -1;
}

void DrawMenu(Menu* menu){
  // 1. Background - The Aged Paper
  ClearBackground(COLOR_PULP_PAPER);

  // Dynamic horizontal line based on screen width
  int margin = 50;
  int uiWidth = SCREEN_WIDTH - (margin * 2);
  
  // 2. Title - The "Ink" look
  DrawText("ARCHAELOGY LOG", margin, 40, 30, COLOR_SUNKEN_INK);
  DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line

  if(menu->count >0){
  
  for (int i = 0; i < menu->count; i++) {
    int itemId = menu->itemIds[i];
    Color textColor = (i == menu->selected) ? COLOR_JADE : COLOR_SUNKEN_INK;
      
    // Draw selection cursor
    if (i == menu->selected) {
      DrawText("> ", 40, 120 + (i * 30), 20, COLOR_JADE);
    }

    
    // DrawRectangle(65, 122 + (i * 30), 4, 16, ITEM_REGISTRY[itemId].rarityColor);
    DrawText(GetName(menu->type, itemId), 75, 120 + (i * 30), 20, textColor);
  }

    // 4. Description Box (Bottom)
    int descBoxHeight = 100;
    int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;
    int selectedId = menu->itemIds[menu->selected];
    DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));
    if(menu->type == ENTITY_ITEM){
      DrawText("ITEM LORE:", margin + 10, descBoxY +10, 15, COLOR_MUSTARD);
      DrawText(GetDescription(menu->type, selectedId), margin+10, descBoxY +40, 18, COLOR_SUNKEN_INK);
    }
    
  }else {
    // 3. What to show when the bag is empty
    DrawText("YOUR LOGBOOK IS EMPTY...", 70, 120, 20, Fade(COLOR_SUNKEN_INK, 0.4f));
  }
}

bool UpdateMenu(Menu* menu, Input* input){
  if(input->buttons_pressed & KEY_W_PRESSED){
    menu->selected--;
    if(menu->selected == -1){
      menu->selected = menu->count-1;
    }
  }
  if(input->buttons_pressed & KEY_S_PRESSED){
    menu->selected++;
    if(menu->selected == menu->count){
      menu->selected = 0;
    }
  }
  if (input->buttons_pressed & INVENTORY_PRESSED) return false;

  return true;
}