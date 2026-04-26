#include "editor/tile_editor.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"
#include "editor/edit_ui.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/register.h"
#include "ui/dialog_box.h"

UIResponse UpdateTileEditor(TileEditor* tile_editor, Input* input){
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        int s = (int)(SCREEN_WIDTH  * 0.66f);

        int tool_selector_x = s+40;
        int tool_selector_y = 40;

        // DrawText("Tile Palette", tool_selector_x, tool_selector_y, 14, COLOR_DUSTY_CORAL);
        Rectangle tile_palette_button = (Rectangle){tool_selector_x, tool_selector_y, 100, 40};
        Rectangle height_adjuster_button = (Rectangle){tool_selector_x + 120, tool_selector_y, 120, 40};
        Rectangle entity_drawer_button = (Rectangle){tool_selector_x + 240, tool_selector_y, 120, 40};
        if(CheckCollisionPointRec(input->mouse, tile_palette_button)){
            tile_editor->tool = TILE_PALETTE;
        }else if(CheckCollisionPointRec(input->mouse, height_adjuster_button)){
            tile_editor->tool = HEIGHT_ADJUSTER;
        }
    }
    switch (tile_editor->tool) {
        case TILE_PALETTE:
            if(UpdateTilePalette(tile_editor,input)){
                return UI_ACTION_EXECUTE;
            }
            break;
        case HEIGHT_ADJUSTER:
            if(UpdateHeightAdjuster(tile_editor,input)){
                return UI_ACTION_EXECUTE;
            }
            break;
        case ENTITY_DRAWER:
            UpdateEntityDrawer(tile_editor,input);
    }

    int s = (int)(SCREEN_WIDTH  * 0.66f);
    Rectangle r = (Rectangle){s,20,(s/2),(SCREEN_HEIGHT *2 /3)};

    // if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, r)){
            return UI_ACTION_CLICK;
        }
    // }
    return UI_ACTION_NONE;
}

void DrawTileEditor(TileEditor* tile_editor){
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    DrawRectangle(s, 20,(s/2), (SCREEN_HEIGHT *2 /3), COLOR_PULP_PAPER);
    DrawRectangleLines(s,20, s/2, (SCREEN_HEIGHT *2 /3), COLOR_SUNKEN_INK);

    int tool_selector_x = s+40;
    int tool_selector_y = 40;
    Rectangle tile_palette_button = (Rectangle){tool_selector_x, tool_selector_y, 140, 40};
    DrawRectangleRec(tile_palette_button, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(tile_palette_button,1.0f, COLOR_SUNKEN_INK);

    Rectangle height_adjuster_button = (Rectangle){tool_selector_x + 120, tool_selector_y, 140, 40};
    DrawRectangleRec(height_adjuster_button, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(height_adjuster_button,1.0f, COLOR_SUNKEN_INK);

    Rectangle entity_drawer_button = (Rectangle){tool_selector_x + 240, tool_selector_y, 140, 40};
    DrawRectangleRec(entity_drawer_button, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(entity_drawer_button,1.0f, COLOR_SUNKEN_INK);
    DrawText("Tile Palette", tile_palette_button.x+10, tile_palette_button.y+15, 14, COLOR_DUSTY_CORAL);
    DrawText("Height Adjuster", height_adjuster_button.x + 10, height_adjuster_button.y + 15, 14, COLOR_DUSTY_SALMON);
    DrawText("Entity Drawer", entity_drawer_button.x + 10, entity_drawer_button.y + 15, 14, COLOR_DUSTY_ROSE);

    switch (tile_editor->tool) {
        case TILE_PALETTE:
            DrawTilePalette(tile_editor);
            break;
        case HEIGHT_ADJUSTER:
            DrawHeightAdjuster();
            break;
        case ENTITY_DRAWER:
            DrawEntityDrawer();
    }
}

void DrawTilePalette(TileEditor* editor) {
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    // DrawDialog(char *name, char *text)
    int startX = s + 40;
    int startY = 100;
    DrawText("Tile Palette", startX, startY, 30, COLOR_DUSTY_CORAL);

    for (int i = 0; i < 8; i++) {

        Rectangle slot = { startX + (i * 50), startY + 60, 40, 40 };
        DrawRectangleRec(slot, TILE_REGISTRY[i].color);

        if(i ==editor->selected_tile_type){
            DrawText(TILE_REGISTRY[i].label, startX, startY+150, 20, COLOR_SUNKEN_INK);
        }
    }

    Rectangle execute_button = (Rectangle){startX, startY+200, 100, 40};
    DrawRectangleRec(execute_button, COLOR_DUSTY_SALMON);
    DrawRectangleLinesEx(execute_button, 1.0f, COLOR_SUNKEN_INK);
    DrawText("Execute", startX+10, startY+215, 14, COLOR_PULP_PAPER);
}

bool UpdateTilePalette(TileEditor* tile_editor, Input* input){
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    // DrawDialog(char *name, char *text)
    int startX = s + 40;
    int startY = 100;
    for (int i = 0; i < 8; i++) {

        Rectangle slot = { startX + (i * 50), startY + 60, 40, 40 };
        if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
            if(CheckCollisionPointRec(input->mouse, slot)){
                tile_editor->selected_tile_type = i;
            }
        }
    }

    Rectangle execute_button = (Rectangle){startX, startY+200, 100, 40};
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, execute_button)){
            return true;
        }
    }

    return false;

}
bool UpdateHeightAdjuster(TileEditor* tile_editor, Input* input){
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    // DrawDialog(char *name, char *text)
    int startX = s + 40;
    int startY = 100;
    Rectangle minus_5 = (Rectangle) {startX, startY + 50, 60, 40};
    Rectangle minus_1 = (Rectangle) {startX+100, startY + 50, 60, 40};
    Rectangle plus_1 = (Rectangle) {startX+200, startY + 50, 60, 40};
    Rectangle plus_5 = (Rectangle){startX+300, startY+50, 60,40};

    tile_editor->height_delta =0;
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, minus_5)) tile_editor->height_delta = -5;
        if(CheckCollisionPointRec(input->mouse, minus_1)) tile_editor->height_delta = -1;
        if(CheckCollisionPointRec(input->mouse, plus_1)) tile_editor->height_delta = +1;
        if(CheckCollisionPointRec(input->mouse, plus_5)) tile_editor->height_delta = +5;
    }
    return tile_editor->height_delta !=0;
}

void UpdateEntityDrawer(TileEditor* tile_editor, Input* input){

}

void DrawHeightAdjuster(){
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    // DrawDialog(char *name, char *text)
    int startX = s + 40;
    int startY = 100;
    DrawText("Height Adjuster", startX, startY, 30, COLOR_DUSTY_SALMON);

    Rectangle minus_5 = (Rectangle) {startX, startY + 50, 60, 40};
    Rectangle minus_1 = (Rectangle) {startX+100, startY + 50, 60, 40};
    Rectangle plus_1 = (Rectangle) {startX+200, startY + 50, 60, 40};
    Rectangle plus_5 = (Rectangle){startX+300, startY+50, 60,40};
    DrawRectangleRec(minus_5, COLOR_RED_OCHRE);
    DrawRectangleLinesEx(minus_5, 1.0f, COLOR_SUNKEN_INK);
    DrawText("-5", minus_5.x+10, minus_5.y+15, 16, COLOR_PULP_PAPER);
    DrawRectangleRec(minus_1, COLOR_POTTERS_PINK);
    DrawRectangleLinesEx(minus_1, 1.0f, COLOR_SUNKEN_INK);
    DrawText("-1", minus_1.x+10, minus_1.y+15, 16, COLOR_PULP_PAPER);
    DrawRectangleRec(plus_1, COLOR_CELADON);
    DrawRectangleLinesEx(plus_1, 1.0f, COLOR_SUNKEN_INK);
    DrawText("+1", plus_1.x+10, plus_1.y+15, 16, COLOR_PULP_PAPER);
    DrawRectangleRec(plus_5, COLOR_VIRIDIAN);
    DrawRectangleLinesEx(plus_5, 1.0f, COLOR_SUNKEN_INK);
    DrawText("+5", plus_5.x+10, plus_5.y+15, 16, COLOR_PULP_PAPER);


    // Rectangle execute_button = (Rectangle){startX, startY+200, 100, 40};
    // DrawRectangleRec(execute_button, COLOR_DUSTY_SALMON);
    // DrawRectangleLinesEx(execute_button, 1.0f, COLOR_SUNKEN_INK);
    // DrawText("Execute", startX+10, startY+215, 14, COLOR_PULP_PAPER);
}
void DrawEntityDrawer(){

}
