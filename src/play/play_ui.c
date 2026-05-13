#include "play/play_ui.h"

#include <math.h>
#include "core/camera_tools.h"
#include "defs/types_engine.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "environment/map.h"
#include "raylib.h"
#include "registry/mineral_register.h"
#include "play/play_inventory.h"
#include "ui/dialog_box.h"
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


void DrawPlaySession(PlaySession* session){
  if(session->state == INVENTORY){
    DrawInventory(&session->menu);
  }else if(session->state==MINERAL_INVENTORY){
    DrawMineralInventory(&session->player);
  }else {
    Draw_Map(&session->map,&session->camera);
    if(session->state == TALKING){
      DrawMessage(&session->manager);
    }else if(session->state == ITEM){
      DrawDialog("you found a christmas present", session->pendingItemName);
    }
  }
}
