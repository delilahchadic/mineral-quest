#include "engine/play_session.h"
#include "core/camera_tools.h"

void InitPlaySession(PlaySession* session){
  session->player = Get_Default_Player();
  session->menu = (Menu){0};

  LoadMap("rr",&session->map);
  InitMap(&session->map);
  InitScriptManager(&session->manager,100);
  CenterCameraOn(&session->camera,session->map.player->position, 2.0f, &session->map);
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
          bool moved = UpdatePhysics(&session->map, &input);
          Update_Map(&session->map, moved);
          float smoothness = 0.1f;
          // 1. Get the base isometric position
          Vector2 isoPos = GetWorldToIso(session->map.player->position);

          // 2. Find what tile the player is on to get the height
          int gridX = (int)(session->map.player->position.x / TILE_SIZE);
          int gridY = (int)(session->map.player->position.y / TILE_SIZE);

          // Clamp them so you don't crash at map edges
          if(gridX < 0) gridX = 0;
          if(gridX >= session->map.columns) gridX = session->map.columns - 1;
          float tileHeight = session->map.grid[gridY][gridX].height;

          // 3. Offset the camera Y by the height (assuming 8 or 16 pixels per height unit)
          // We subtract because "up" on the screen is negative Y
          float visualOffset = tileHeight * 8.0f;
          isoPos.y -= visualOffset;

          // 4. Smoothly follow the adjusted position
          session->camera.target.x += (isoPos.x - session->camera.target.x) * smoothness;
          session->camera.target.y += (isoPos.y - session->camera.target.y) * smoothness;
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
