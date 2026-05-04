#include "editor/edit_session.h"

#include <dirent.h>

#include "raylib.h"
#include "raymath.h"

#include "defs/types_ui.h"
#include "editor/tile_editor.h"
#include "core/camera_tools.h"
#include "defs/constants.h"
#include "defs/types_core.h"
#include "defs/types_ui.h"
#include "editor/edit_form.h"
#include "editor/edit_ui.h"
#include "editor/edit_loader.h"
#include "editor/selection_tools.h"
#include "editor/tile_editor.h"
#include "engine/palette.h"
#include "environment/map.h"
#include "environment/map_loader.h"
#include "registry/command_register.h"
#include "ui/dialog_box.h"


void InitEditSession(EditSession* session){
  session->menu = (SystemMenu){0};
  session->map = (Map){0};
  session->form = (EditorForm){0};
  session->buffer = (SelectionBuffer){0};
  session->editor = (TileEditor){0};
  LoadFiles(session);
  session->form.active_field = FIELD_NAME;
  int s = (int)(SCREEN_WIDTH  * 0.66f);
  session->current_tile_panel = (Rectangle){s,(SCREEN_HEIGHT *2 /3) + 30 ,(SCREEN_WIDTH * 0.33f)-0, (SCREEN_HEIGHT * 0.33f) -60};
  FillSystemMenu(&session->menu, (int[]){3,4,2},3, "Edit Mode");
  CenterCameraOn(&session->camera,(Vector2){0,0}, 0.75f, &session->map );
  InitEditSessionMouseTracking(session);
  InitTileEditor(&session->editor);
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
        if(!UpdateTileEditor(&session->editor, &session->map, &session->buffer, input, &session->camera)){
            UpdateSelectionBuffer(session,input);
        }
        UpdateEditorCamera(session,input);
    }
    break;
    case LOAD_PROMPT:{
        if(UpdateLoadPrompt(session,input)){
            LoadMap(session->loaded_files.names[session->loaded_files.current_file], &session->map);
            InitSelection(&session->buffer, session->map.columns, session->map.rows);
            session->state = EDITOR;
        }
        break;
    }
  default:
    break;
  }
  if(input->buttons_pressed &BACKSPACE_PRESSED){
      memset(&session->map, 0, sizeof(Map));
      session->state = EDITOR_MENU;
  }
  return true;
}

void CloseEditor(EditSession* session){
    Close_Map(&session->map);
    CloseSelection(&session->buffer);
    free(session);
}
