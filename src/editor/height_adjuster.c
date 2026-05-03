#include "editor/height_adjuster.h"
#include "core/selection_buffer.h"
#include "engine/palette.h"
#include "ui/ui_helpers.h"

void InitHeightAdjuster(HeightAdjuster* adjuster, int start_x, int start_y){
    adjuster->minus_5 = (Rectangle) {start_x, start_y + 50, 60, 40};
    adjuster->minus_1 = (Rectangle) {start_x+100, start_y + 50, 60, 40};
    adjuster->plus_1 = (Rectangle) {start_x+200, start_y + 50, 60, 40};
    adjuster->plus_5 = (Rectangle){start_x+300, start_y+50, 60,40};

    adjuster->minus_25 = (Rectangle) {start_x, start_y + 100, 60, 40};
    adjuster->minus_10 = (Rectangle) {start_x+100, start_y + 100, 60, 40};
    adjuster->plus_10 = (Rectangle) {start_x+200, start_y + 100, 60, 40};
    adjuster->plus_25 = (Rectangle){start_x+300, start_y+100, 60,40};

    adjuster->zero = (Rectangle) {start_x, start_y + 150, 60, 40};
    adjuster->twenty_five = (Rectangle) {start_x+100, start_y + 150, 60, 40};
    adjuster->fifty = (Rectangle) {start_x+200, start_y + 150, 60, 40};
    adjuster->one_hundred = (Rectangle){start_x+300, start_y+150, 60,40};

    adjuster->minus_8 = (Rectangle) {start_x, start_y + 200, 60, 40};
    adjuster->plus_8 = (Rectangle) {start_x+100, start_y + 200, 60, 40};
}

void SetSelectionHeight(Map* map, SelectionBuffer* buffer, int height){
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->columns; x++) {
            if (IsTileSelected(buffer, x, y)) {
                map->grid[y][x].height = height;
            }
        }
    }
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
    int height = -1;
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_5)) height_delta = -5;
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_1)) height_delta = -1;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_1)) height_delta = +1;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_5)) height_delta = +5;
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_10)) height_delta = -10;
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_25)) height_delta = -25;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_10)) height_delta = +10;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_25)) height_delta = +25;
        if(CheckCollisionPointRec(input->mouse, adjuster->zero)) height = 0;
        if(CheckCollisionPointRec(input->mouse, adjuster->twenty_five)) height = 25;
        if(CheckCollisionPointRec(input->mouse, adjuster->fifty)) height = 50;
        if(CheckCollisionPointRec(input->mouse, adjuster->one_hundred)) height = 100;
        if(CheckCollisionPointRec(input->mouse, adjuster->minus_8)) height_delta = -8;
        if(CheckCollisionPointRec(input->mouse, adjuster->plus_8)) height_delta = +8;
    }
    if(height_delta !=0){
        AdjustSelectionHeight(map,buffer, height_delta);
    }
    if(height > -1){
        SetSelectionHeight(map, buffer, height);
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
    DrawButton(adjuster->minus_25, "-25", COLOR_MAGENTA, COLOR_PULP_PAPER);
    DrawButton(adjuster->minus_10, "-10", COLOR_ROSE, COLOR_PULP_PAPER);
    DrawButton(adjuster->plus_10, "+10", COLOR_PERYLENE_RED, COLOR_PULP_PAPER);
    DrawButton(adjuster->plus_25, "+25", COLOR_NEON_CRIMSON, COLOR_PULP_PAPER);
    DrawButton(adjuster->zero, "0", COLOR_CERULEAN_DUSTY, COLOR_PULP_PAPER);
    DrawButton(adjuster->twenty_five, "25", COLOR_COBALT_BLUE, COLOR_PULP_PAPER);
    DrawButton(adjuster->fifty, "50", COLOR_ULTRAMARINE, COLOR_PULP_PAPER);
    DrawButton(adjuster->one_hundred, "100", COLOR_PRUSSIAN_BLUE, COLOR_PULP_PAPER);
    DrawButton(adjuster->minus_8, "-8", COLOR_NICKEL_TITANITE, COLOR_PULP_PAPER);
    DrawButton(adjuster->plus_8, "+8", COLOR_AMBER, COLOR_PULP_PAPER);
}
