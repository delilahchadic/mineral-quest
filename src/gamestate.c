#include "gamestate.h"
void InitGame(Gamestate* gamestate){
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
    }
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
  }
}
void DrawGameplay(Gamestate* gamestate){
    Draw_World(&gamestate->world);
    Draw_Player(&gamestate->player); // Our "Hero"
    Draw_Character(&gamestate->character);
    DrawText("The world awaits...", 20, 20, 20, PINK);
    if (gamestate->manager.active) {
      Draw_Dialog(&gamestate->manager); 
    }
}

void DrawScreen(Gamestate* gamestate){
    switch(gamestate->screen) {
      case LOGO:
        DrawText("YOUR STUDIO LOGO", 250, 200, 20, LIGHTGRAY);
        break;
      case TITLE:
        DrawText("Mineral Quest", 220, 150, 40, BLACK);
        DrawText("PRESS ENTER TO START", 280, 250, 20, DARKGRAY);
        break;
      case GAMEPLAY:
        DrawGameplay(gamestate);
        break;
    }
}
void CloseGame(Gamestate* gamestate){
  Close_Dialog_Manager(&gamestate->manager);
  Close_Character(&gamestate->character);
  Close_Player(&gamestate->player);
  CloseWindow();
}