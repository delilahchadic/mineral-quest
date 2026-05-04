#include "engine/play_session.h"
#include "core/camera_tools.h"
#include "defs/types_engine.h"
#include "raylib.h"

void InitPlaySession(PlaySession* session){
  session->state = ADVENTURE;
  session->player = Get_Default_Player();
  session->menu = (Menu){0};
  LoadMap("rivers",&session->map);
  InitMap(&session->map);
  InitScriptManager(&session->manager,100);
  CenterCameraOn(&session->camera,session->map.player->position,3.0f, &session->map);
  int tx = (int)(session->map.player->position.x / TILE_SIZE);
  int ty = (int)(session->map.player->position.y / TILE_SIZE);

      // Set the altitude to the floor height immediately
    float startFloor = session->map.grid[ty][tx].height * 8.0f;
    session->map.player->altitude = startFloor;
}

void UpdatePlaySession(PlaySession* session){
  Input input = CaptureInput();
  switch(session->state){
    case ADVENTURE:
        if(input.buttons_pressed & INVENTORY_PRESSED){
          session->menu.type = ENTITY_ITEM;
          FillMenu(&session->menu, &session->player.inventory.itemIds, session->player.inventory.count);
          session->state = INVENTORY;
        } else if(input.buttons_pressed & INTERACT_PRESSED){
          int item = PollChest(&session->player,&session->map);
          if(item > 0){
            session->state = ITEM;
            sprintf(session->pendingItemName,"You got a %s !", GetName(ENTITY_ITEM, item));
          }else{
            InitDialog(&session->map, &session->manager);
            if(session->manager.active) session->state = TALKING;
          }
        }else{
            float dt = GetFrameTime();
                    if (dt > 0.1f) dt = 0.1f;

                    // 1. RUN PHYSICS FIRST
                    UpdatePhysics(&session->map, &input);

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

        }
      AdjustCamera(session, false);
      break;
    case INVENTORY:
      UpdateInventory(session, &input);
      break;
    case TALKING:
      UpdateScriptManager(&session->manager, &input);
      session->state = session->manager.active ? TALKING : ADVENTURE;
      AdjustCamera(session,true);
      break;
    case ITEM:
      if(input.buttons_pressed & INTERACT_PRESSED){
        session->state= ADVENTURE;
      }
  }
}
  //if dialog manager is active input is disabled
void DrawPlaySession(PlaySession* session){
  if(session->state == INVENTORY){
    DrawInventory(&session->menu);
  }else{
    Draw_Map(&session->map,&session->camera);
    if(session->state == TALKING){
      DrawMessage(&session->manager);
    }else if(session->state == ITEM){
      DrawDialog("you found a christmas present", session->pendingItemName);
    }
  }
}


/// @brief manages input while game is in inventory mode
/// @param gamestate
void UpdateInventory(PlaySession* session, Input* input){
  if(!UpdateMenu(&session->menu, input)){
    session->state = ADVENTURE;
  }
}

void AdjustCamera(PlaySession* session, bool dialog){
  if(dialog){
    session->camera.zoom += (3.2f - session->camera.zoom) * 0.05f;
  }else{
    session->camera.zoom += (3.0f - session->camera.zoom) * 0.05f;
  }
}

void DrawInventory(Menu* menu) {
  DrawMenu(menu);
}

void InitDialog(Map* map, ScriptManager* manager){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_TALK, 50.0f);
  if(p){
    int dialogID = GetDialogID(ENTITY_CHARACTER, p->id);
    SetActiveMessage(manager, dialogID);
    manager->active = true;
  }
}

int PollChest(Player* player,Map* map){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_GATHER, 50.0f);
  if(p && p->type == ENTITY_ITEM){

    GiveItem(player, p->id);
    Remove_Entity(map, p);
    return p->id;
  }
  return -1;
}
