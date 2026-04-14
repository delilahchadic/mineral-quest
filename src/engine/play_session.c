#include "engine/play_session.h"

void InitPlaySession(PlaySession* session){
  session->player = Get_Default_Player();
  LoadMap("highway",&session->map);
  InitMap(&session->map);
  Init_Dialog_Manager(&session->manager,100);
}

void UpdatePlaySession(PlaySession* session){
  switch(session->state){
    case ADVENTURE:
      if(!session->manager.active){
        Input input = CaptureInput();

        if(input.inventory){
          session->state = INVENTORY;
        } else if(input.dialog){
          InitDialog(&session->map, &session->manager);
        }else{
          bool moved = UpdatePhysics(&session->map, input.dir, input.jump);
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
      UpdateInventory(session);
  }
  //if dialog manager is active input is disabled
  
    
}

/// @brief manages input while game is in inventory mode
/// @param gamestate 
void UpdateInventory(PlaySession* session){
  if(IsKeyPressed(KEY_W)){
    session->player.inventory.selected--;
    if(session->player.inventory.selected == -1){
      session->player.inventory.selected = session->player.inventory.count-1;
    }
  }
  if(IsKeyPressed(KEY_S)){
    session->player.inventory.selected++;
    if(session->player.inventory.selected == session->player.inventory.count){
      session->player.inventory.selected = 0;
    }
  }
  if (IsKeyPressed(KEY_I)) session->state = ADVENTURE;
    
}

void DrawInventory(PlaySession* session) {
    // 1. Background - The Aged Paper
    ClearBackground(COLOR_PULP_PAPER);

    // Dynamic horizontal line based on screen width
    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);
    
    // 2. Title - The "Ink" look
    DrawText("ARCHAELOGY LOG", margin, 40, 30, COLOR_SUNKEN_INK);
    DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line

    if(session->player.inventory.count >0){
      // 3. List Items
      for (int i = 0; i < session->player.inventory.count; i++) {
        int itemId = session->player.inventory.itemIds[i];
        Color textColor = (i == session->player.inventory.selected) ? COLOR_JADE : COLOR_SUNKEN_INK;
          
        // Draw selection cursor
        if (i == session->player.inventory.selected) {
          DrawText("> ", 40, 120 + (i * 30), 20, COLOR_JADE);
        }

        
        // DrawRectangle(65, 122 + (i * 30), 4, 16, ITEM_REGISTRY[itemId].rarityColor);
        DrawText(ITEM_REGISTRY[itemId].name, 75, 120 + (i * 30), 20, textColor);
      }

      // 4. Description Box (Bottom)
      int descBoxHeight = 100;
      int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;
      int selectedId = session->player.inventory.itemIds[session->player.inventory.selected];
      DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));
      DrawText("ITEM LORE:", margin + 10, descBoxY +10, 15, COLOR_MUSTARD);
      DrawText(ITEM_REGISTRY[selectedId].description, margin+10, descBoxY +40, 18, COLOR_SUNKEN_INK);
    }else {
        // 3. What to show when the bag is empty
        DrawText("YOUR LOGBOOK IS EMPTY...", 70, 120, 20, Fade(COLOR_SUNKEN_INK, 0.4f));
    }
}

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
    DrawInventory(session);
  default:
    break;
  }
  
}

void InitDialog(Map* map, DialogManager* manager){
  // get characterid
  // set dialg
  int id = PollDialog(map);
  if(id >=0){
    Set_Active_Dialog(manager, id);
    manager->active = true;
  }
}

