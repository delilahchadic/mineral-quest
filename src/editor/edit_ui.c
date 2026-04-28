#include "editor/edit_ui.h"

#include "editor/edit_form.h"
#include "editor/tile_editor.h"
#include "raylib.h"
#include "ui/ui_helpers.h"

void DrawEditSession(EditSession* session){
  switch (session->state){
  case EDITOR:
    Draw_Map(&session->map, &session->camera);
    DrawSelectionOverlay(&session->buffer,&session->map,session->camera);
    DrawTileEditor(&session->editor);
    break;
  case EDITOR_MENU:
    DrawSystemMenu(&session->menu);
    break;
  case EDITOR_PROMPT:
    DrawEditForm(&session->form);
    break;
  default:
    break;
  }
}

void UpdateEditorCamera(EditSession* session,Input *input){
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    float speedMultiplier = 1.0f / session->camera.zoom;
        float currentSpeed = 1000.0f * speedMultiplier;
    if (input->buttons_pressed & MOVEMENT_PRESSED) {
      float length = (input->dir.x != 0 && input->dir.y != 0) ? 0.707f : 1.0f;
      session->camera.target.x += input->dir.x * length * currentSpeed * dt;
      session->camera.target.y += input->dir.y * length * currentSpeed * dt;
    }

    if(IsKeyPressed(KEY_DOWN)){
        if(session->camera.zoom <= 0.5){
            session->camera.zoom -= 0.05;
        }else{
            session->camera.zoom -= 0.5;
        }

    }

    if(IsKeyPressed(KEY_UP)){
        session->camera.zoom += 0.5;

    }
  }
