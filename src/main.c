#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include "player.h"
#include "world.h"
#include <math.h>
#include "character.h"
#include "dialog.h"
#include <stdio.h>

// Simple state to track our game "scene"
typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY } GameScreen;

void UpdateGameplay(Player* player, World* world,
  Character* character, Dialog_Manager* manager){
    Handle_Input(player, world);
    if(!manager->active){
      if(Vector2Distance(player->position, character->position) <50.f){
        if (IsKeyPressed(KEY_E) && !manager->active) {
            // Tell the manager to look up the ID stored on the character
            Set_Active_Dialog(manager, character->dialogId);
            manager->active = true;
        }
      }
    }
    if(manager->active){
      Update_Dialog_Manager(manager);
    }
    
}

int main(void) {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Mineral Quest");
    SetTargetFPS(60); 

    // System Setup
    GameScreen currentScreen = LOGO;
    int framesCounter = 0;

    Player player = Get_Default_Player();
    Character character = GetCharacterDefault();
    World world = Init_World();

    Dialog_Manager manager;
    Init_Dialog_Manager(&manager,100);
    Load_Dialogs_From_CSV(&manager,"dialog.csv");

    // Main game loop
    while (!WindowShouldClose()) {
      // --- 1. Update Logic ---
      switch(currentScreen) {
        case LOGO:
          framesCounter++;
          if (framesCounter > 120) currentScreen = TITLE; // Switch after 2 seconds
          break;
        case TITLE:
          if (IsKeyPressed(KEY_ENTER)) currentScreen = GAMEPLAY;
          break;
        case GAMEPLAY:
          UpdateGameplay(&player,&world,&character, &manager);
          break;
      }

        // --- 2. Drawing ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch(currentScreen) {
                case LOGO:
                    DrawText("YOUR STUDIO LOGO", 250, 200, 20, LIGHTGRAY);
                    break;
                case TITLE:
                    DrawText("Mineral Quest", 220, 150, 40, BLACK);
                    DrawText("PRESS ENTER TO START", 280, 250, 20, DARKGRAY);
                    break;
                case GAMEPLAY:
                    Draw_World(&world);
                    Draw_Player(&player); // Our "Hero"
                    Draw_Character(&character);
                    DrawText("The world awaits...", 20, 20, 20, PINK);
                    if (manager.active) {
                      Draw_Dialog(&manager); 
                    }
                    break;
            }
        EndDrawing();
    }
    Close_Dialog_Manager(&manager);
    // UnloadTexture(sprite);
    UnloadTexture(character.sprite);
    // De-Initialization
    CloseWindow();

    return 0;
}