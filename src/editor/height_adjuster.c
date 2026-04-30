#include "editor/height_adjuster.h"
#include "core/selection_buffer.h"
#include "engine/palette.h"
#include "ui/ui_helpers.h"

void InitHeightAdjuster(HeightAdjuster* adjuster, int start_x, int start_y){
    adjuster->minus_5 = (Rectangle) {start_x, start_y + 50, 60, 40};
    adjuster->minus_1 = (Rectangle) {start_x+100, start_y + 50, 60, 40};
    adjuster->plus_1 = (Rectangle) {start_x+200, start_y + 50, 60, 40};
    adjuster->plus_5 = (Rectangle){start_x+300, start_y+50, 60,40};
}

void AdjustSelectionHeight(Map* map, SelectionBuffer* buffer, int height_delta){
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->columns; x++) {
            if (IsTileSelected(buffer, x, y)) {
                map->grid[y][x].height += height_delta;
            }
        }
    }
}

void UpdateHeightAdjuster(HeightAdjuster* adjuster, Map* map, SelectionBuffer* buffer, Input* input){
    int height_delta =0;
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_5)) height_delta = -5;
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_1)) height_delta = -1;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_1)) height_delta = +1;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_5)) height_delta = +5;
    }
    if(height_delta !=0){
        AdjustSelectionHeight(map,buffer, height_delta);
    }
}


void DrawHeightAdjuster(HeightAdjuster* adjuster){
    int s = (int)(SCREEN_WIDTH  * 0.66f);

    int startX = s + 40;
    int startY = 100;

    DrawText("Height Adjuster", startX, startY, 30, COLOR_DUSTY_SALMON);
    DrawButton(adjuster->minus_5, "-5", COLOR_RED_OCHRE, COLOR_PULP_PAPER);
    DrawButton(adjuster->minus_1, "-1", COLOR_POTTERS_PINK, COLOR_PULP_PAPER);
    DrawButton(adjuster->plus_1, "+1", COLOR_CELADON, COLOR_PULP_PAPER);
    DrawButton(adjuster->plus_5, "+5", COLOR_VIRIDIAN, COLOR_PULP_PAPER);
}
