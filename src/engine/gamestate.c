#include "engine/gamestate.h"

void InitGame(Gamestate* gamestate){
  // System Setup
  gamestate->screen= LOGO;
  gamestate->framesCounter = 0; 
  InitPlaySession(&gamestate->session);
}

//routes to update based on gamestate
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
      UpdatePlaySession(&gamestate->session);
      break;
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
        DrawPlaySession(&gamestate->session);
        break;
    }
}

void CloseGame(Gamestate* gamestate){
  Close_Player(PLAYER);
  Close_Map(&gamestate->session.map);
  free(gamestate);
  CloseWindow();
}