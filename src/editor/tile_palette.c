#include "editor/tile_palette.h"
#include "defs/types_ui.h"
#include "raylib.h"
#include "registry/register.h"
#include "core/selection_buffer.h"

// Define total registered count dynamically based on TileType enum length
#define PALETTE_COLUMNS 7
#define PALETTE_ROWS 6
#define PALETTE_TOTAL_TILES 42

void InitTilePalette(TilePalette* palette, int start_x, int start_y){
    for (int i = 0; i < PALETTE_ROWS; i++) {
        for(int j = 0; j < PALETTE_COLUMNS; j++){
            int idx = j + (i * PALETTE_COLUMNS);
            if (idx >= PALETTE_TOTAL_TILES) break;

            Rectangle slot = { start_x + (j * 50), start_y + (i * 50), 42, 42 };
            palette->tile_type_buttons[idx] = slot;
            palette->colors[idx] = TILE_REGISTRY[idx].color;
        }
    }
    // Place execute button clearly below row 5 (start_y + 250 + margin)
    palette->execute_button = (Rectangle){start_x, start_y + 340, 120, 36};
}

void DrawTilePalette(TilePalette* palette) {
    int s = (int)(GetScreenWidth() * 0.66f);
    int startX = s + 40;

    // Y position dynamically relative to palette top instead of arbitrary scalar
    int labelY = palette->tile_type_buttons[0].y - 30;

    DrawText("Tile Palette", startX, labelY - 40, 24, COLOR_DUSTY_CORAL);

    for(int i = 0; i < PALETTE_TOTAL_TILES; i++){
        DrawRectangleRec(palette->tile_type_buttons[i], TILE_REGISTRY[i].color);

        // Draw selection highlight border
        if(i == palette->selected_tile_type){
            DrawRectangleLinesEx(palette->tile_type_buttons[i], 2.0f, COLOR_SUNKEN_INK);
            DrawText(TILE_REGISTRY[i].label, startX, labelY, 18, COLOR_SUNKEN_INK);
        }
    }

    DrawRectangleRec(palette->execute_button, COLOR_DUSTY_SALMON);
    DrawRectangleLinesEx(palette->execute_button, 1.0f, COLOR_SUNKEN_INK);
    DrawText("Execute", palette->execute_button.x + 12, palette->execute_button.y + 10, 14, COLOR_PULP_PAPER);
}

void SetSelectionTileType(Map* map, SelectionBuffer* buffer, int selected_tile_type){
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->columns; x++) {
            if (IsTileSelected(buffer, x, y)) {
                map->grid[y][x].type = selected_tile_type;
            }
        }
    }
}

void UpdateTilePalette(TilePalette* palette, Map* map, SelectionBuffer* buffer, Input* input){
    if (!(input->buttons_pressed & LEFT_MOUSE_CLICKED)) return;

    // Check execute button collision first
    if (CheckCollisionPointRec(input->mouse, palette->execute_button)) {
        SetSelectionTileType(map, buffer, palette->selected_tile_type);
        return; // Consume click so it does not alter map selections
    }

    // Check tile button collisions
    for (int i = 0; i < PALETTE_TOTAL_TILES; i++) {
        if (CheckCollisionPointRec(input->mouse, palette->tile_type_buttons[i])) {
            palette->selected_tile_type = i;
            return; // Consume click
        }
    }
}
