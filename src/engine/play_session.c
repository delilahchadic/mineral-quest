#include "engine/play_session.h"
#include "core/camera_tools.h"

void InitPlaySession(PlaySession* session){
  session->player = Get_Default_Player();
  session->menu = (Menu){0};

  LoadMap("highway 101",&session->map);
  InitMap(&session->map);
  InitScriptManager(&session->manager,100);
  CenterCameraOn(&session->camera,session->map.player->position,2.0f, &session->map);
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
          // bool moved = UpdatePhysics(&session->map, &input);
          // Update_Map(&session->map, moved);
          // // Inside UpdatePlaySession, replace the camera logic:
          float dt = GetFrameTime();
          float lerpSpeed = 10.0f; // Adjust this: higher = snappier, lower = smoother
          // 1. Resolve ALL physics first
          UpdatePhysics(&session->map, &input);
          // 2. NOW calculate camera based on the FINAL position for this frame
          Vector2 targetIsoPos = GetWorldToIso(session->map.player->position);

          int gridX = (int)(session->map.player->position.x / TILE_SIZE);
          int gridY = (int)(session->map.player->position.y / TILE_SIZE);
          gridX = Clamp(gridX, 0, session->map.columns - 1);
          gridY = Clamp(gridY, 0, session->map.rows - 1);

          // USE A LERP FOR THE HEIGHT TOO
          // If you snap the height, the camera "pops" when crossing tiles.
          // static float smoothTileHeight = 0.0f;
          // float targetTileHeight = session->map.grid[gridY][gridX].height * 8.0f;
          // smoothTileHeight += (targetTileHeight - smoothTileHeight) * (1.0f - expf(-5.0f * dt));

          static float smoothTileHeight = -1.0f; // Initialize to an impossible value
          float targetTileHeight = session->map.grid[gridY][gridX].height * 8.0f;

          // If this is the first run, snap immediately to avoid the "elevator" effect
          if (smoothTileHeight < 0) {
              smoothTileHeight = targetTileHeight;
          } else {
              // Otherwise, lerp smoothly as we walk/jump
              smoothTileHeight += (targetTileHeight - smoothTileHeight) * (1.0f - expf(-5.0f * dt));
          }

          // float totalOffset = smoothTileHeight + session->map.player->jumpoffset;
          float totalOffset = smoothTileHeight + session->map.player->jumpoffset;
          targetIsoPos.y -= totalOffset;

          // 3. Final Camera Lerp
          float dist = Vector2Distance(session->camera.target, targetIsoPos);

          if (dist > 2.0f) { // Only move if more than 2 pixels away
              float blend = 1.0f - expf(-lerpSpeed * dt);
              session->camera.target.x += (targetIsoPos.x - session->camera.target.x) * blend;
              session->camera.target.y += (targetIsoPos.y - session->camera.target.y) * blend;
          } else {
              session->camera.target = targetIsoPos; // Snap the last tiny bit
          }
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
    session->camera.zoom += (2.2f - session->camera.zoom) * 0.05f;
  }else{
    session->camera.zoom += (2.0f - session->camera.zoom) * 0.05f;
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
