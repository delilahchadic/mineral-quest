#include "engine/gamestate.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/register.h"
#include "registry/register_loader.h"
#include <time.h>

int main(void) {
    // Begin Engine
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mineral Quest");
    SetTargetFPS(TARGET_FPS);
    InitAudioDevice();
    Gamestate *gamestate = calloc(1, sizeof(Gamestate));
    InitGame(gamestate);
    InitRegistries();
    srand(time(NULL));
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

    CloseGame(gamestate);
    CloseRegistries();
    return 0;
}
