#include "ui/menu.h"
#include <string.h>
#include <stdio.h>
#include "registry/command_interface.h"
#include "registry/register.h"

void FillMenu(Menu* menu, int itemIds[], int count){
    memset(menu->itemIds, 0, sizeof(menu->itemIds));
    memcpy(menu->itemIds, itemIds, sizeof(int) * count);
    menu->count = count;
    menu->selected = count > 0 ? 0 : -1;
}

void FillSystemMenu(SystemMenu* menu, int ids[], int count, char* name){
  memcpy(&menu->ids, ids, sizeof(int) * count);
  strncpy(menu->name, name, sizeof(menu->name) - 1);
  menu->name[sizeof(menu->name) - 1] = '\0';
  menu->count = count;
  menu->selected = count > 0 ? 0 : -1;
}

void DrawSystemMenu(SystemMenu* menu){
  // 1. Background - The Aged Paper
  ClearBackground(COLOR_PULP_PAPER);

  // Dynamic horizontal line based on screen width
  int margin = 50;
  int uiWidth = SCREEN_WIDTH - (margin * 2);

  // 2. Title - The "Ink" look
  DrawText(menu->name, margin, 40, 30, COLOR_DUSTY_ROSE);
  DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line

  if(menu->count > 0){

    for (int i = 0; i < menu->count; i++) {
      int itemId = menu->ids[i];
      Color textColor = (i == menu->selected) ? COLOR_CERULEAN_CORE : COLOR_SUNKEN_INK;

      // Draw selection cursor
      if (i == menu->selected) {
        DrawText("> ", 40, 120 + (i * 30), 20, COLOR_CERULEAN_CORE);
      }

      DrawText(GetCommandLabel(itemId), 75, 120 + (i * 30), 20, textColor);
    }

    // 4. Description Box (Bottom)
    int descBoxHeight = 100;
    int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;
    int selectedId = menu->ids[menu->selected];
    DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));

    DrawText(GetCommandDescription(selectedId), margin+10, descBoxY +40, 18, COLOR_SUNKEN_INK);

  } else {
    // 3. What to show when the bag is empty
    DrawText("YOUR LOGBOOK IS EMPTY...", 70, 120, 20, Fade(COLOR_SUNKEN_INK, 0.4f));
  }
}

void DrawMenu(const MenuRenderData* data) {
    // 1. Background
    ClearBackground(COLOR_PULP_PAPER);

    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);

    // 2. Title & Line
    DrawText(data->title, margin, 40, 30, COLOR_SUNKEN_INK);
    DrawRectangle(margin, 80, uiWidth, 2, COLOR_SUNKEN_INK);

    // 3. Item List
    if (data->count > 0) {
        for (int i = 0; i < data->count; i++) {
            Color textColor = (i == data->selected) ? COLOR_JADE : COLOR_SUNKEN_INK;

            if (i == data->selected) {
                DrawText("> ", 40, 120 + (i * 30), 20, COLOR_JADE);
            }

            DrawText(data->item_labels[i], 75, 120 + (i * 30), 20, textColor);
        }

        // 4. Bottom Info Panel
        int descBoxHeight = 120;
        int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;

        DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));

        // Left Side: Lore
        if (data->lore_title && data->lore_body) {
            DrawText(data->lore_title, margin + 10, descBoxY + 10, 15, COLOR_DUSTY_ROSE);
            DrawText(data->lore_body, margin + 10, descBoxY + 35, 18, COLOR_SUNKEN_INK);
        }

        // Right Side: Context Tag & Stats
        int stats_x = margin + (uiWidth / 2) + 50;
        int stats_y = descBoxY + 10;

        if (data->context_tag && data->context_tag[0] != '\0') {
            DrawText(data->context_tag, stats_x, stats_y, 14, COLOR_DUSTY_ROSE);
            stats_y += 20;
        }

        DrawText("EFFECTS:", stats_x, stats_y, 15, COLOR_DUSTY_ROSE);
        stats_y += 22;

        if (data->effect_count > 0 && data->effect_lines) {
            for (int e = 0; e < data->effect_count; e++) {
                DrawText(data->effect_lines[e], stats_x, stats_y, 16, COLOR_SUNKEN_INK);
                stats_y += 18;

                if (stats_y > descBoxY + descBoxHeight - 15) break;
            }
        } else {
            DrawText("None", stats_x, stats_y, 16, Fade(COLOR_SUNKEN_INK, 0.5f));
        }
    } else {
        DrawText(data->empty_message, 70, 120, 20, Fade(COLOR_SUNKEN_INK, 0.4f));
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
  if (input->buttons_pressed & menu->exit_button) return false;

  return true;
}

int UpdateSystemMenu(SystemMenu* menu, Input* input){
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
  if (input->buttons_pressed & ENTER_PRESSED) return menu->ids[menu->selected];

  return -1;
}
