#include "engine/gamestate.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/register.h"
#include "registry/register_loader.h"

int main(void) {
  //Begin Engine
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mineral Quest");
  SetTargetFPS(TARGET_FPS);
  Gamestate* gamestate = calloc(1, sizeof(Gamestate));
  InitGame(gamestate);
  InitAudioDevice();
  InitRegistries();
  // Main game loop
  while (!WindowShouldClose()) {
    // --- 1. Update Logic ---
    UpdateScene(gamestate);
    // --- 2. Drawing ---
    BeginDrawing();
      ClearBackground(COLOR_PULP_PAPER);
      DrawScreen(gamestate);
    EndDrawing();
  }
  UnloadSound(MINERAL_SOUND);
  CloseGame(gamestate);
  CloseRegistries();
  return 0;
}
