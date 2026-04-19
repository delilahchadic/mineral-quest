#include "engine/play_session.h"

void InitPlaySession(PlaySession* session){
  session->player = Get_Default_Player();
  session->menu = (Menu){0};
  LoadMap("highway",&session->map);
  InitMap(&session->map);
  InitScriptManager(&session->manager,100);
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
          PollChest(&session->player,&session->map);
          InitDialog(&session->map, &session->manager);
          if(session->manager.active) session->state = TALKING;
        }else{
          bool moved = UpdatePhysics(&session->map, &input);
          Update_Map(&session->map, moved);
        }
      AdjustCamera(&session->map, false);
      break;
    case INVENTORY:
      UpdateInventory(session, &input);
      break;
    case TALKING:
      UpdateScriptManager(&session->manager, &input);
      session->state = session->manager.active ? TALKING : ADVENTURE;
      AdjustCamera(&session->map,true);
      break;
  }
}
  //if dialog manager is active input is disabled
void DrawPlaySession(PlaySession* session){
  switch (session->state)
  {
  case ADVENTURE:
    Draw_Map(&session->map);    
    break;
  case INVENTORY:
    DrawInventory(&session->menu);
    break;
  case TALKING:
    Draw_Map(&session->map);    
    DrawMessage(&session->manager);
    
  default:
    break;
  }
  
    
}


/// @brief manages input while game is in inventory mode
/// @param gamestate 
void UpdateInventory(PlaySession* session, Input* input){
  if(!UpdateMenu(&session->menu, input)){
    session->state = ADVENTURE;
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

void PollChest(Player* player,Map* map){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_GATHER, 50.0f);
  if(p && p->type == ENTITY_ITEM){
    
    GiveItem(player, p->id);
    TraceLog(LOG_INFO,"hello");
    Remove_Entity(map, p);
  }
}

