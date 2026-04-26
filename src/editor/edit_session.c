#include "editor/edit_session.h"

#include "defs/types_ui.h"
#include "editor/tile_editor.h"
#include "raylib.h"
#include "raymath.h"

#include "core/camera_tools.h"
#include "defs/constants.h"
#include "defs/types_core.h"
#include "defs/types_ui.h"
#include "editor/edit_form.h"
#include "editor/edit_ui.h"
#include "editor/selection_tools.h"
#include "editor/tile_editor.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "registry/command_register.h"
#include "ui/dialog_box.h"


void InitEditSession(EditSession* session){
  session->menu = (SystemMenu){0};
  session->map = (Map){0};
  session->form = (EditorForm){0};
  session->buffer = (SelectionBuffer){0};
  session->editor = (TileEditor){0};

  session->form.active_field = FIELD_NAME;
  FillSystemMenu(&session->menu, (int[]){3,2},2, "Edit Mode");
  CenterCameraOn(&session->camera,(Vector2){0,0}, 1.0f);
  InitEditSessionMouseTracking(session);
}

void InitEditSessionMouseTracking(EditSession* session){
    session->isDragging = false;
    session->dragStart.x = -1;
    session->dragStart.y = -1;
    session->last_selected_tile.x = -1;
    session->last_selected_tile.y = -1;
}

bool UpdateEditSession(EditSession* session, Input* input){
  switch (session->state){
  case EDITOR_MENU:{
    int command = UpdateSystemMenu(&session->menu, input);
    if(command == 2) return false;
    if(command > 0){
      ExecuteCommand(command, session);
    }}
    break;
  case EDITOR_PROMPT:
    if(UpdateEditForm(&session->form, &session->map, &session->buffer, input)){
        session->state = EDITOR;
    }
    break;
    case EDITOR:{
        UIResponse response = UpdateTileEditor(&session->editor, input);
        if(response == UI_ACTION_NONE){
            UpdateSelectionBuffer(session,input);
        }
        UpdateEditorCamera(session,input);
        if(response == UI_ACTION_EXECUTE){
            if(session->editor.tool == TILE_PALETTE){
                SetSelectionTileType(session);
            }else if(session->editor.tool == HEIGHT_ADJUSTER){
                AdjustSelectionHeight(session);
            }
        }
    }

    break;
  default:
    break;
  }
  return true;
}

void SetSelectionTileType(EditSession* session){
    for (int y = 0; y < session->map.rows; y++) {
        for (int x = 0; x < session->map.columns; x++) {

            // Only draw if the bit is 1
            if (IsTileSelected(&session->buffer, x, y)) {
                session->map.grid[y][x].type = session->editor.selected_tile_type;
            }
        }
    }
}

void AdjustSelectionHeight(EditSession* session){
    for (int y = 0; y < session->map.rows; y++) {
        for (int x = 0; x < session->map.columns; x++) {

            // Only draw if the bit is 1
            if (IsTileSelected(&session->buffer, x, y)) {
                session->map.grid[y][x].height += session->editor.height_delta;
            }
        }
    }
}

void CloseEditor(EditSession* session){
    Close_Map(&session->map);
    CloseSelection(&session->buffer);
}
