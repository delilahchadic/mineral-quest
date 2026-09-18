#include "play/play_ui.h"

#include <math.h>
#include "core/camera_tools.h"
#include "defs/constants.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "environment/map_ui.h"
#include "raylib.h"
#include "registry/mineral_register.h"
#include "play/play_inventory.h"
#include "registry/register.h"
#include "ui/dialog_box.h"
#include "ui/equip_menu.h"
#include "ui/node_menu.h"
#include "ui/stats_menu.h"
#include "systems/script_manager.h"
#include "systems/player.h"

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

void DrawHUD(){
    Vector2 hud_start = (Vector2){SCREEN_WIDTH * 0.9, SCREEN_HEIGHT * 0.78};
    Vector2 max_hp_start = (Vector2){(hud_start.x + (RUSTY_HEADSHOT_SPRITE.width * 0.5))-PLAYER->stats.max_hp,(hud_start.y + (RUSTY_HEADSHOT_SPRITE.height * 0.5))+10};
    Vector2 hp_start = (Vector2){(hud_start.x + (RUSTY_HEADSHOT_SPRITE.width * 0.5))-PLAYER->stats.current_hp,(hud_start.y + (RUSTY_HEADSHOT_SPRITE.height * 0.5))+10};

    DrawTextureEx(RUSTY_HEADSHOT_SPRITE, hud_start, 0.0, 0.5, WHITE);
    DrawRectangle(max_hp_start.x, max_hp_start.y, PLAYER->stats.max_hp, 6, COLOR_SUNKEN_INK);
    DrawRectangle(hp_start.x, hp_start.y, PLAYER->stats.current_hp, 6, COLOR_MAY_GREEN);
}

void DrawPlaySession(PlaySession* session){
    if(session->state==NODE_MENU){
        DrawNodeSession(session);
    }else if(session->state==EQUIPMENT_MENU){
        DrawEquipmentMenu(session,&session->equip_menu);
    }else if(session->state == STATS_MENU){
      DrawStatsMenu(session);
  }else if(session->state == INVENTORY){
    DrawInventory(&session->menu);
    DrawHUD();
  }else if(session->state==MINERAL_INVENTORY){
    DrawMineralInventory(session->player);
  }else if(session->state == LEVEL_INVENTORY){
    DrawLevelInventory();
  }else {
    Draw_Map(&session->map,&session->camera);
    DrawHUD();
    if(session->state == TALKING){
      DrawMessage(&session->manager);
    }
    if(session->state == ITEM){
      DrawDialog("you found a christmas present", session->pendingItemName);
    }
  }
}
