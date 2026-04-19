#include "engine/play_session.h"

void InitPlaySession(PlaySession* session){
  session->player = Get_Default_Player();
  session->menu = (Menu){0};
  LoadMap("highway",&session->map);
  InitMap(&session->map);
  Init_Dialog_Manager(&session->manager,100);
}

void UpdatePlaySession(PlaySession* session){
  Input input = CaptureInput();
  switch(session->state){
    case ADVENTURE:
      if(!session->manager.active){
        

        if(input.buttons_pressed & INVENTORY_PRESSED){
          session->menu.type = ENTITY_ITEM;
          FillMenu(&session->menu, &session->player.inventory.itemIds, session->player.inventory.count);
          session->state = INVENTORY;
        } else if(input.buttons_pressed & INTERACT_PRESSED){
          InitDialog(&session->map, &session->manager);
        }else{
          bool moved = UpdatePhysics(&session->map, &input);
          Update_Map(&session->map, moved);
        }
      }
      //control camera if manager is active or not
      if(session->manager.active){
        Update_Dialog_Manager(&session->manager);
        AdjustCamera(&session->map,true);
      } else{
        AdjustCamera(&session->map, false);
      }
      break;
    case INVENTORY:
      
      UpdateInventory(session, &input);
  }
}
  //if dialog manager is active input is disabled
void DrawPlaySession(PlaySession* session){
  switch (session->state)
  {
  case ADVENTURE:
  
    Draw_Map(&session->map);    
    DrawText("The world awaits...", 20, 20, 20, COLOR_DUSTY_CORAL);
    if (session->manager.active) {
      Draw_Dialog(&session->manager); 
    }
    break;
  case INVENTORY:
    DrawInventory(&session->menu);
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

void InitDialog(Map* map, DialogManager* manager){
  // get characterid
  // set dialg
  MapEntity* p = PollTrait(map, TRAIT_TALK, 50.0f);
  if(p){
    int dialogID = GetDialogID(ENTITY_CHARACTER, p->id);
    Set_Active_Dialog(manager, dialogID);
    manager->active = true;
  }
}

