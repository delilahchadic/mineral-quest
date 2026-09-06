#include "editor/tile_editor.h"

#include "defs/constants.h"
#include "defs/types_core.h"
#include "defs/types_systems.h"
#include "editor/building_builder.h"
#include "environment/map_loader.h"
#include "defs/types_ui.h"
#include "editor/entity_drawer.h"
#include "editor/height_adjuster.h"
#include "editor/tile_palette.h"
#include "editor/edit_ui.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/register.h"
#include "ui/dialog_box.h"
#include "ui/ui_helpers.h"

void InitTileEditor(TileEditor* tile_editor){
    int s = (int)(SCREEN_WIDTH  * 0.66f);
    int startX = s + 40;
    int startY = 240;

    tile_editor->panel = (Rectangle){s,20,(s/2),(SCREEN_HEIGHT *2 /3) };
    tile_editor->save_button = (Rectangle){startX, startY+340, 100, 40};

    int tool_selector_x = s+40;
    int tool_selector_y = 40;
    tile_editor->tile_palette_button = (Rectangle){tool_selector_x, tool_selector_y, 120, 40};
    tile_editor->height_adjuster_button = (Rectangle){tool_selector_x + 120, tool_selector_y, 120, 40};
    tile_editor->entity_drawer_button = (Rectangle){tool_selector_x + 240, tool_selector_y, 120, 40};
    tile_editor->building_builder_button = (Rectangle){tool_selector_x + 240, tool_selector_y+40, 120, 40};
    InitTilePalette(&tile_editor->palette, startX, startY);
    InitHeightAdjuster(&tile_editor->adjuster, startX, startY);
    InitEnitityDrawer(&tile_editor->drawer, startX, 130);
    InitBuildingBuilder(&tile_editor->builder, startX, startY);
}

bool UpdateTileEditor(TileEditor* tile_editor, Map* map, SelectionBuffer* buffer, Input* input, Camera2D* camera,int last_x, int last_y){
    if(input->buttons_pressed & LEFT_MOUSE_CLICKED){
        if(CheckCollisionPointRec(input->mouse, tile_editor->tile_palette_button)){
            tile_editor->tool = TILE_PALETTE;
        }else if(CheckCollisionPointRec(input->mouse, tile_editor->height_adjuster_button)){
            tile_editor->tool = HEIGHT_ADJUSTER;
        }else if(CheckCollisionPointRec(input->mouse, tile_editor->entity_drawer_button)){
            tile_editor->tool = ENTITY_DRAWER;
        }else if(CheckCollisionPointRec(input->mouse, tile_editor->building_builder_button)){
            tile_editor->tool = BUIDING_BUILDER;
        }else if(CheckCollisionPointRec(input->mouse, tile_editor->save_button)){
            SaveMap(map);
        }
    }
    switch (tile_editor->tool) {
        case TILE_PALETTE:
            UpdateTilePalette(&tile_editor->palette, map, buffer, input);
            break;
        case HEIGHT_ADJUSTER:
            UpdateHeightAdjuster(&tile_editor->adjuster, map, buffer, input);
            break;
        case ENTITY_DRAWER:
            UpdateEnitityDrawer(&tile_editor->drawer, map, input,camera);
            return true;
            break;
        case BUIDING_BUILDER:
            UpdateBuildingBuilder(&tile_editor->builder, map, input, last_x, last_y);
        default:
            break;
    }

    return(CheckCollisionPointRec(input->mouse, tile_editor->panel));
}

void DrawTileEditor(TileEditor* tile_editor){

    DrawRectangleRec(tile_editor->panel, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(tile_editor->panel, 1.0f, COLOR_SUNKEN_INK);

    DrawButton(tile_editor->tile_palette_button,"Tile Palette",COLOR_PULP_PAPER, COLOR_DUSTY_CORAL);
    DrawButton(tile_editor->height_adjuster_button,"Height Adjuster",COLOR_PULP_PAPER, COLOR_DUSTY_SALMON);
    DrawButton(tile_editor->entity_drawer_button,"Entity Drawer",COLOR_PULP_PAPER, COLOR_DUSTY_ROSE);
    DrawButton(tile_editor->building_builder_button,"Building Builder",COLOR_PULP_PAPER, COLOR_CERULEAN_CORE);

    switch (tile_editor->tool) {
        case TILE_PALETTE:
            DrawTilePalette(&tile_editor->palette);
            break;
        case HEIGHT_ADJUSTER:
            DrawHeightAdjuster(&tile_editor->adjuster);
            break;
        case ENTITY_DRAWER:
            DrawEnitityDrawer(&tile_editor->drawer);
            break;
        case BUIDING_BUILDER:
            DrawBuildingBuilder(&tile_editor->builder);
            break;
    }

    DrawButton(tile_editor->save_button, "Save", COLOR_PULP_PAPER, COLOR_GREEN_GOLD);
}
