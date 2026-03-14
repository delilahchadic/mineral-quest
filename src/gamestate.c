#include "gamestate.h"

void InitGame(Gamestate* gamestate){

  // System Setup
  LoadItemRegistry();
  LoadDialogRegistry();
  gamestate->screen= LOGO;
  gamestate->framesCounter = 0; 
  gamestate->player = Get_Default_Player();
  gamestate->character = GetCharacterDefault();
  gamestate->world = Init_World();

  // Camera Setup
  gamestate->camera.target = gamestate->player.position;
  gamestate->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };// Center of the 800x450 screen
  gamestate->camera.rotation = 0.0f;
  gamestate->camera.zoom = 1.0f;
  
  Init_Dialog_Manager(&gamestate->manager,100);
}

void UpdateGameplay(Gamestate* gamestate){

    if(!gamestate->manager.active){
      Handle_Input(&gamestate->player, &gamestate->world);
      if(Vector2Distance(gamestate->player.position, gamestate->character.position) <50.f){
        if (IsKeyPressed(KEY_E) && !gamestate->manager.active) {
            // Tell the manager to look up the ID stored on the character
            Set_Active_Dialog(&gamestate->manager, gamestate->character.dialogId);
            gamestate->manager.active = true;
        }
      }

      if(IsKeyPressed(KEY_I)){
        gamestate->screen = INVENTORY;
      }
    }

    float smoothness = 0.1f;
    gamestate->camera.target.x += (gamestate->player.position.x - gamestate->camera.target.x) * smoothness;
    gamestate->camera.target.y += (gamestate->player.position.y - gamestate->camera.target.y) * smoothness;

    if(gamestate->manager.active){
      Update_Dialog_Manager(&gamestate->manager);
      gamestate->camera.zoom += (1.2f - gamestate->camera.zoom) * 0.05f;
    } else{
      gamestate->camera.zoom += (1.0f - gamestate->camera.zoom) * 0.05f;
    }
    
}

void UpdateInventory(Gamestate* gamestate){
  if(IsKeyPressed(KEY_W)){
    gamestate->player.inventory.selected--;
    if(gamestate->player.inventory.selected == -1){
      gamestate->player.inventory.selected = gamestate->player.inventory.count-1;
    }
  }
  if(IsKeyPressed(KEY_S)){
    gamestate->player.inventory.selected++;
    if(gamestate->player.inventory.selected == gamestate->player.inventory.count){
      gamestate->player.inventory.selected = 0;
    }
  }
  if (IsKeyPressed(KEY_I)) gamestate->screen = GAMEPLAY;
    
}

void UpdateScene(Gamestate* gamestate){
  switch(gamestate->screen) {
    case LOGO:
      gamestate->framesCounter++;
      if (gamestate->framesCounter > 120) gamestate->screen = TITLE; // Switch after 2 seconds
      break;
    case TITLE:
      if (IsKeyPressed(KEY_ENTER)) gamestate->screen = GAMEPLAY;
      break;
    case GAMEPLAY:
      UpdateGameplay(gamestate);
      break;
    case INVENTORY:
      UpdateInventory(gamestate);
      break;
  }
}
void DrawGameplay(Gamestate* gamestate){
    BeginMode2D(gamestate->camera);
      Draw_World(&gamestate->world);
      Draw_Player(&gamestate->player); // Our "Hero"
      Draw_Character(&gamestate->character);
    EndMode2D();
    
    DrawText("The world awaits...", 20, 20, 20, COLOR_DUSTY_CORAL);
    if (gamestate->manager.active) {
      Draw_Dialog(&gamestate->manager); 
    }
}

void DrawInventory(Gamestate* gamestate) {
    // 1. Background - The Aged Paper
    ClearBackground(COLOR_PULP_PAPER);

    // Dynamic horizontal line based on screen width
    int margin = 50;
    int uiWidth = SCREEN_WIDTH - (margin * 2);
    
    // 2. Title - The "Ink" look
    DrawText("ARCHAELOGY LOG", margin, 40, 30, COLOR_SUNKEN_INK);
    DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line

    if(gamestate->player.inventory.count >0){
      // 3. List Items
      for (int i = 0; i < gamestate->player.inventory.count; i++) {
        int itemId = gamestate->player.inventory.itemIds[i];
        Color textColor = (i == gamestate->player.inventory.selected) ? COLOR_JADE : COLOR_SUNKEN_INK;
          
        // Draw selection cursor
        if (i == gamestate->player.inventory.selected) {
          DrawText("> ", 40, 120 + (i * 30), 20, COLOR_JADE);
        }

        
        // DrawRectangle(65, 122 + (i * 30), 4, 16, ITEM_REGISTRY[itemId].rarityColor);
        DrawText(ITEM_REGISTRY[itemId].name, 75, 120 + (i * 30), 20, textColor);
      }

      // 4. Description Box (Bottom)
      int descBoxHeight = 100;
      int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;
      int selectedId = gamestate->player.inventory.itemIds[gamestate->player.inventory.selected];
      DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));
      DrawText("ITEM LORE:", margin + 10, descBoxY +10, 15, COLOR_MUSTARD);
      DrawText(ITEM_REGISTRY[selectedId].description, margin+10, descBoxY +40, 18, COLOR_SUNKEN_INK);
    }else {
        // 3. What to show when the bag is empty
        DrawText("YOUR LOGBOOK IS EMPTY...", 70, 120, 20, Fade(COLOR_SUNKEN_INK, 0.4f));
    }
}

void DrawScreen(Gamestate* gamestate){
    switch(gamestate->screen) {
      case LOGO:{
            // 1. Define the colors in the order you want them to appear
          Color barPalette[] = {
              COLOR_BEAVIS_SHIRT, COLOR_VHS_BLUE, COLOR_JADE, 
              COLOR_TAROT_GOLD, COLOR_DUSTY_CORAL, COLOR_DUSTY_SALMON, 
              COLOR_DUSTY_ROSE, COLOR_TEXAS_HAZE
          };
          int colorCount = 8;
          float barWidth = SCREEN_WIDTH / colorCount;

            // 2. Draw the vertical bars
          for (int i = 0; i < colorCount; i++) {
              DrawRectangle(i * barWidth, 0, barWidth,SCREEN_HEIGHT, barPalette[i]);
          }

            // 3. 
            // Draw a dark semi-transparent bar behind the text for readability
            DrawRectangle(0, SCREEN_HEIGHT/2 - 40, SCREEN_WIDTH, 80, Fade(COLOR_SUNKEN_INK, 0.6f));
            DrawText("ARCHAEOLOGY", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 - 20, 30, COLOR_PULP_PAPER);
            
            break;
      }
      case TITLE:
        DrawText("Mineral Quest", 220, 150, 40, COLOR_SUNKEN_INK);
        DrawText("PRESS ENTER TO START", 280, 250, 20, COLOR_SUNKEN_INK);
        break;
      case GAMEPLAY:
        DrawGameplay(gamestate);
        break;
      case INVENTORY:
        DrawInventory(gamestate);
        break;
    }
}

void CloseGame(Gamestate* gamestate){
  Close_Character(&gamestate->character);
  Close_Player(&gamestate->player);
  CloseWindow();
}