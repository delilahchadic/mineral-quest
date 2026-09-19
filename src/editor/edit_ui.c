#include "editor/edit_ui.h"

#include <dirent.h>
#include "defs/types_engine.h"
#include "defs/types_ui.h"
#include "editor/edit_form.h"
#include "editor/tile_editor.h"
#include "engine/palette.h"
#include "raylib.h"
#include "registry/register.h"
#include "ui/ui_helpers.h"
#include "ui/menu.h"
#include "editor/edit_loader.h"
#include "environment/map_ui.h"

void DrawCurrentPanel(Gamestate* gamestate){
    EditSession* session = &gamestate->edit_session;
    DrawRectangleRec(session->current_tile_panel, COLOR_PULP_PAPER);
    DrawRectangleLinesEx(session->current_tile_panel, 1.0f, COLOR_SUNKEN_INK);
    DrawText("Current Tile", session->current_tile_panel.x+10, session->current_tile_panel.y+15, 15.0f, COLOR_SUNKEN_INK);
    if(session->last_selected_tile.x >= 0){
        DrawText(TILE_REGISTRY[ (gamestate->map.grid[(int)session->last_selected_tile.y][(int)session->last_selected_tile.x].type)].label, session->current_tile_panel.x+10, session->current_tile_panel.y+30, 15.0f, COLOR_SUNKEN_INK);
        char curr_pos[10];
        char height[10];
        snprintf(height, sizeof(height), "%d", gamestate->map.grid[(int)session->last_selected_tile.y][(int)session->last_selected_tile.x].height);
        snprintf(curr_pos, sizeof(curr_pos), "%d , %d", (int)session->last_selected_tile.x, (int)session->last_selected_tile.y);
        DrawText(curr_pos, session->current_tile_panel.x+10, session->current_tile_panel.y+45, 15.0f, COLOR_SUNKEN_INK);
        DrawText(height, session->current_tile_panel.x+10, session->current_tile_panel.y+60, 15.0f, COLOR_SUNKEN_INK);
        Rectangle r = {session->current_tile_panel.x+80,session->current_tile_panel.y+45,40,40};
        DrawRectangleRec(r, TILE_REGISTRY[ (gamestate->map.grid[(int)session->last_selected_tile.y][(int)session->last_selected_tile.x].type)].color);
    }
}

void DrawEditSession(Gamestate* gamestate){
    EditSession* session = &gamestate->edit_session;
    switch (session->state){
        case EDITOR:
            Draw_Map(&gamestate->map, &gamestate->camera,false);
            DrawSelectionOverlay(&session->buffer,&gamestate->map,gamestate->camera);
            DrawTileEditor(&session->editor);
            DrawText(gamestate->map.name, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT / 13.0f,30,COLOR_DUSTY_ROSE);
            if(session->editor.tool != ENTITY_DRAWER) DrawCurrentPanel(gamestate);
            break;
        case EDITOR_MENU:
            DrawSystemMenu(&session->menu);
            break;
        case EDITOR_PROMPT:
            DrawEditForm(&session->form);
            break;
        case LOAD_PROMPT:
            DrawLoadForm(session);
        default:
            break;
  }

}

void UpdateEditorCamera(Gamestate* gamestate,Input *input){
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    float speedMultiplier = 1.0f / gamestate->camera.zoom;
        float currentSpeed = 1000.0f * speedMultiplier;
    if (input->buttons_pressed & MOVEMENT_PRESSED) {
      float length = (input->dir.x != 0 && input->dir.y != 0) ? 0.707f : 1.0f;
      gamestate->camera.target.x += input->dir.x * length * currentSpeed * dt;
      gamestate->camera.target.y += input->dir.y * length * currentSpeed * dt;
    }

    if(IsKeyPressed(KEY_DOWN)){
        if(gamestate->camera.zoom <= 0.5f){
            gamestate->camera.zoom -= 0.05f;
            if (gamestate->camera.zoom < 0.05f) gamestate->camera.zoom = 0.05f; // Clamp
        } else {
            gamestate->camera.zoom -= 0.25f;
        }
    }

    if(IsKeyPressed(KEY_UP)){
        gamestate->camera.zoom += 0.25;

    }
  }
