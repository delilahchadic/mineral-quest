#include "gamestate.h"
#include "palette.h"
int main(void) {
    // Initialization
    Gamestate gamestate;
    InitGame(&gamestate);

    // Main game loop
    while (!WindowShouldClose()) {
      // --- 1. Update Logic ---
      UpdateScene(&gamestate);
        // --- 2. Drawing ---
        BeginDrawing();
          ClearBackground(COLOR_PULP_PAPER);
          DrawScreen(&gamestate);
        EndDrawing();
    }
    CloseGame(&gamestate);
    return 0;
}