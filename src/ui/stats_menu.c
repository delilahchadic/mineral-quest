#include "ui/stats_menu.h"
#include <stdio.h>
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "defs/types_entities.h"
#include "registry/register.h"

void UpdateStatsMenu(PlaySession* session, Input* input){
  if(input->buttons_pressed & KEY_P_PRESSED){
    session->state = ADVENTURE;
  }
}

void DrawStatsMenu(PlaySession* session){
      // 1. Background - The Aged Paper
      ClearBackground(COLOR_CERULEAN_COBALT);

      // Dynamic horizontal line based on screen width
      int margin = 50;
      int uiWidth = SCREEN_WIDTH - (margin * 2);

      // 2. Title - The "Ink" look
      DrawText("Stats", margin, 40, 30, COLOR_BONE_WHITE);
      DrawRectangle(50, 80, uiWidth, 2, COLOR_BONE_WHITE); // A simple line

      DrawText("Base", 250,90,15,COLOR_BONE_WHITE);
      DrawText("Current", 450,90,15,COLOR_BONE_WHITE);
      for(int i = 0;i< STAT_COUNT;i++){
          char base[5],current[5];
          sprintf(base, "%d", session->map.player->stats->base[i]);
          sprintf(current,"%d",session->map.player->stats->current[i]);
          // sprintf(base, "%d", session->player.stats.base[i]);
                    // sprintf(current, "%d", session->player.stats.current[i]);
          DrawText(STATS_NAMES[i], margin, 120 + (i * 30), 15, COLOR_BONE_WHITE);
          DrawText(base,250, 120 + (i * 30), 20, COLOR_BONE_WHITE);
          DrawText(current,450, 120 + (i * 30), 20, COLOR_BONE_WHITE);
      }
}
