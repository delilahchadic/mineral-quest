#include "editor/tile_palette.h"

#include "defs/types_ui.h"
#include "raylib.h"

#include "registry/register.h"
#include "core/selection_buffer.h"

void InitTilePalette(TilePalette* palette, int start_x, int start_y){
    for (int i = 0; i < 4; i++) {
        for(int j = 0; j < 5; j++){
            Rectangle slot = { start_x + (j * 50), start_y + (i * 60), 40, 40 };
            palette->tile_type_buttons[j + (i*5)] = slot;
            palette->colors[j + (i*5)] =  TILE_REGISTRY[j + (i*5)].color;
        }
    }
    palette->execute_button = (Rectangle){start_x, start_y+280, 100, 40};
}

void DrawTilePalette(TilePalette* palette) {
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    int startX = s + 40;
    int startY = (palette->tile_type_buttons[0].height) * 10;
    DrawText("Tile Palette", startX, 100, 30, COLOR_DUSTY_CORAL);

    for(int i =0;i<20;i++){
        DrawRectangleRec(palette->tile_type_buttons[i],TILE_REGISTRY[i].color );
        if(i ==palette->selected_tile_type){
            DrawText(TILE_REGISTRY[i].label, startX, startY, 20, COLOR_SUNKEN_INK);
        }
    }

    // Rectangle execute_button =
    DrawRectangleRec(palette->execute_button, COLOR_DUSTY_SALMON);
    DrawRectangleLinesEx(palette->execute_button, 1.0f, COLOR_SUNKEN_INK);
    DrawText("Execute", palette->execute_button.x + 10, palette->execute_button.y + 20, 14, COLOR_PULP_PAPER);
}

void SetSelectionTileType(Map* map, SelectionBuffer* buffer, int selected_tile_type){
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->columns; x++) {

            // Only draw if the bit is 1
            if (IsTileSelected(buffer, x, y)) {
                map->grid[y][x].type = selected_tile_type;
            }
        }
    }
}

void UpdateTilePalette(TilePalette* palette, Map* map, SelectionBuffer* buffer, Input* input){

    for(int i =0;i<20;i++){
        if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
            if(CheckCollisionPointRec(input->mouse, palette->tile_type_buttons[i])){
                palette->selected_tile_type = i;
            }
        }
    }

    // Rectangle execute_button = (Rectangle){startX, startY+300, 100, 40};
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, palette->execute_button)){
            SetSelectionTileType(map, buffer, palette->selected_tile_type);
        }
    }
}
