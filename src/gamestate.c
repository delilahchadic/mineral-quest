#include "gamestate.h"

void InitGame(Gamestate* gamestate){

  //sets screen width, inits window, sets target fps
  const int screenWidth = 800;
  const int screenHeight = 450;
  InitWindow(screenWidth, screenHeight, "Mineral Quest");
  SetTargetFPS(60); 
  
  // System Setup
  gamestate->screen= LOGO;
  gamestate->framesCounter = 0; 
  gamestate->player = Get_Default_Player();
  gamestate->character = GetCharacterDefault();
  gamestate->world = Init_World();

  // Camera Setup
  gamestate->camera.target = gamestate->player.position;
  gamestate->camera.offset = (Vector2){ 400.0f, 225.0f }; // Center of the 800x450 screen
  gamestate->camera.rotation = 0.0f;
  gamestate->camera.zoom = 1.0f;

  Init_Dialog_Manager(&gamestate->manager,100);
  Load_Dialogs_From_CSV(&gamestate->manager,"dialog.csv"); 
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
    }
    
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
      if (IsKeyPressed(KEY_I)) gamestate->screen = GAMEPLAY;
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
      gamestate->camera.zoom += (1.2f - gamestate->camera.zoom) * 0.05f;
      Draw_Dialog(&gamestate->manager); 
    }else{
      gamestate->camera.zoom += (1.0f - gamestate->camera.zoom) * 0.05f;
    }
}

void DrawInventory(Gamestate* gamestate) {
    // 1. Background - The Aged Paper
    ClearBackground(COLOR_PULP_PAPER);
    
    // 2. Title - The "Ink" look
    DrawText("ARCHAELOGY LOG", 50, 40, 30, COLOR_SUNKEN_INK);
    DrawRectangle(50, 80, 700, 2, COLOR_SUNKEN_INK); // A simple line

    // 3. List Items
    for (int i = 0; i < gamestate->player.inventory.count; i++) {
        int itemId = gamestate->player.inventory.itemIds[i];
        Color textColor = (i == gamestate->player.inventory.selected) ? COLOR_JADE : COLOR_SUNKEN_INK;
        
        // Draw selection cursor
        if (i == gamestate->player.inventory.selected) {
            DrawText("> ", 40, 120 + (i * 30), 20, COLOR_JADE);
        }
        
        DrawText(ITEM_DB[itemId].name, 70, 120 + (i * 30), 20, textColor);
    }

    // 4. Description Box (Bottom)
    int selectedId = gamestate->player.inventory.itemIds[gamestate->player.inventory.selected];
    DrawRectangle(50, 300, 700, 100, Fade(COLOR_SUNKEN_INK, 0.1f));
    DrawText("ITEM LORE:", 60, 310, 15, COLOR_MUSTARD);
    DrawText(ITEM_DB[selectedId].description, 60, 340, 18, COLOR_SUNKEN_INK);
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
          float barWidth = (float)GetScreenWidth() / colorCount;

            // 2. Draw the vertical bars
          for (int i = 0; i < colorCount; i++) {
              DrawRectangle(i * barWidth, 0, barWidth, GetScreenHeight(), barPalette[i]);
          }

            // 3. 
            // Draw a dark semi-transparent bar behind the text for readability
            DrawRectangle(0, GetScreenHeight()/2 - 40, GetScreenWidth(), 80, Fade(COLOR_SUNKEN_INK, 0.6f));
            DrawText("ARCHAEOLOGY", GetScreenWidth()/2 - 180, GetScreenHeight()/2 - 20, 30, COLOR_PULP_PAPER);
            
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
  Close_Dialog_Manager(&gamestate->manager);
  Close_Character(&gamestate->character);
  Close_Player(&gamestate->player);
  CloseWindow();
}