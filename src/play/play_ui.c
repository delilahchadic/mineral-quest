#include "play/play_ui.h"

#include <math.h>
#include "core/camera_tools.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "environment/map_ui.h"
#include "raylib.h"
#include "registry/mineral_register.h"
#include "play/play_inventory.h"
#include "registry/register.h"
#include "ui/dialog_box.h"
#include "ui/equip_menu.h"
#include "systems/script_manager.h"

void AdjustCamera(PlaySession* session, bool dialog, float dt){
    float max_w = (session->map.columns - 1) * TILE_SIZE;
    float max_h = (session->map.rows - 1) * TILE_SIZE;

    if (session->map.player->position.x < 0) session->map.player->position.x = 0;
    if (session->map.player->position.y < 0) session->map.player->position.y = 0;
    if (session->map.player->position.x > max_w) session->map.player->position.x = max_w;
    if (session->map.player->position.y > max_h) session->map.player->position.y = max_h;
    // 2. FIND THE EXACT POSITION OF THE PLAYER'S FEET
    Vector2 playerIso = GetWorldToIso(session->map.player->position);

    // The player is drawn at playerIso.y - altitude.
    // We want the camera to center on the player's sprite, not the floor.
    Vector2 targetPos = playerIso;
    targetPos.y -= session->map.player->altitude; // ONLY subtract altitude

    // 3. ONE SINGLE LERP
    float camSpeed = 15.0f;
    // float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;
    float lerpFactor = 1.0f - expf(-camSpeed * dt);

    session->camera.target.x += (targetPos.x - session->camera.target.x) * lerpFactor;
    session->camera.target.y += (targetPos.y - session->camera.target.y) * lerpFactor;

  if(dialog){
    session->camera.zoom += (3.2f - session->camera.zoom) * 0.05f;
  }else{
    session->camera.zoom += (3.0f - session->camera.zoom) * 0.05f;
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



void DrawPlaySession(PlaySession* session){

    if(session->state==EQUIPMENT_MENU){
        DrawEquipmentMenu(session,&session->equip_menu);
    }else if(session->state == STATS_MENU){
      DrawStatsMenu(session);
  }else if(session->state == INVENTORY){
    DrawInventory(&session->menu);
  }else if(session->state==MINERAL_INVENTORY){
    DrawMineralInventory(session->player);
  }else if(session->state == LEVEL_INVENTORY){
    DrawLevelInventory();
  }else {
    Draw_Map(&session->map,&session->camera);
    if(session->state == TALKING){
      DrawMessage(&session->manager);
    }
    if(session->state == ITEM){
      DrawDialog("you found a christmas present", session->pendingItemName);
    }
  }
}
