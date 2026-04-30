#include "ui/ui_helpers.h"
#include "defs/constants.h"
#include "environment/map.h"
#include "core/selection_buffer.h"
#include "raylib.h"

void ProcessTextInput(char* buffer, int* count, int maxLen, bool numbersOnly) {
    int key = GetCharPressed();

    while (key > 0) {
        bool isValid = false;

        if (numbersOnly) {
            // Only '0'-'9' (ASCII 48-57)
            if (key >= 48 && key <= 57) isValid = true;
        } else {
            // Printable characters (ASCII 32-125)
            if (key >= 32 && key <= 125) isValid = true;
        }

        // Only add if it's valid AND we have room (saving 1 slot for '\0')
        if (isValid && (*count < maxLen)) {
            buffer[*count] = (char)key;
            buffer[*count + 1] = '\0';
            (*count)++;
        }
        key = GetCharPressed();
    }

    // Single Backspace logic handles everything
    if (IsKeyPressed(KEY_BACKSPACE) && *count > 0) {
        (*count)--;
        buffer[*count] = '\0';
    }
}

void DrawButton(Rectangle button, char* label, Color buttonColor, Color textColor){
    DrawRectangleRec(button, buttonColor);
    DrawRectangleLinesEx(button,1.0f, COLOR_SUNKEN_INK);
    DrawText(label, button.x+10, button.y+15, 14, textColor);
}

void DrawTileHighlight(Vector2 top, Color color) {
    float hw = TILE_SIZE / 1.0f;
    float hh = TILE_SIZE / 2.0f;

    Vector2 right  = { top.x + hw, top.y + hh };
    Vector2 bottom = { top.x,      top.y + TILE_SIZE };
    Vector2 left   = { top.x - hw, top.y + hh };

    // Draw the "Floor" of the highlight
    DrawTriangleFan((Vector2[]){ top, left, bottom, right }, 4, color);
}

void DrawCoolHighlight(Vector2 top, Color baseColor) {
    float time = GetTime();
    // Faster, sharper pulse for a "humming" energy feel
    float pulse = (sinf(time * 10.0f) * 0.1f) + 0.9f;

    // We want the core to be bright, but the aura to be soft
    Color auraColor = Fade(baseColor, 0.2f * pulse);
    Color coreColor = Fade(baseColor, 0.5f * pulse);
    Color lineHighlight = Fade(WHITE, 0.8f * pulse);

    // 1. Draw the "Aura" - slightly larger than the tile
    // We fake this by drawing the diamond shifted 2 pixels out in every direction
    // or just drawing it twice with a small offset.
    DrawTileHighlight(top, auraColor);

    // 2. Draw the Core (This is your main color)
    DrawTileHighlight(top, coreColor);

    // 3. The "Light Edge" - This makes it look like glass or energy
    // Draw only the top-left and top-right lines in WHITE
    float hw = TILE_SIZE / 1.0f;
    float hh = TILE_SIZE / 2.0f;
    Vector2 right  = { top.x + hw, top.y + hh };
    Vector2 left   = { top.x - hw, top.y + hh };

    DrawLineEx(top, right, 2.0f, lineHighlight);
    DrawLineEx(top, left, 2.0f, lineHighlight);
}

void DrawSelectionOverlay(SelectionBuffer* buffer, Map* map, Camera2D camera) {
    BeginMode2D(camera);
    BeginBlendMode(BLEND_ADDITIVE);
    Vector2 tl_corner = GetScreenToWorld2D((Vector2){0,0}, camera);
    Vector2 tr_corner = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,0}, camera);
    Vector2 bl_corner = GetScreenToWorld2D((Vector2){0,SCREEN_HEIGHT}, camera);
    Vector2 br_corner = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,SCREEN_HEIGHT}, camera);

    Vector2 g1 = GetIsoWorldToGrid(tl_corner);
    Vector2 g2 = GetIsoWorldToGrid(tr_corner);
    Vector2 g3 = GetIsoWorldToGrid(bl_corner);
    Vector2 g4 = GetIsoWorldToGrid(br_corner);
    float min_x = fminf(fminf(g1.x, g2.x), fminf(g3.x, g4.x)) -2;
    float max_x = fmaxf(fmaxf(g1.x, g2.x), fmaxf(g3.x, g4.x))+2;

    float min_y = fminf(fminf(g1.y, g2.y), fminf(g3.y, g4.y))-2;
    float max_y = fmaxf(fmaxf(g1.y, g2.y), fmaxf(g3.y, g4.y))+2;

    min_x = min_x < 0 ? 0: min_x;
    min_y = min_y < 0 ? 0: min_y;
    max_x = max_x > map->columns ? map->columns: max_x;
    max_y = max_y > map->rows ? map->rows: max_y;
    // Use the same min_x, max_x logic from before
    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {

            // Only draw if the bit is 1
            if (IsTileSelected(buffer, x, y)) {
                Vector2 g1 = GetWorldToIso((Vector2){ x * TILE_SIZE, y * TILE_SIZE });

                            // 2. Account for the height (h) so it sits on top
                            float h = map->grid[y][x].height * 8.0f;
                            Vector2 top_corner = { g1.x, g1.y - h };

                            // 3. Draw the highlight using the TOP corner as the start
                            // Note: I modified this to ensure it matches your grid lines
                            DrawCoolHighlight(top_corner,COLOR_BEAVIS_SHIRT);
            }
        }
    }
    EndBlendMode();
  EndMode2D();
}
