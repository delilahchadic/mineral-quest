#include "editor/edit_ui.h"

#include "editor/edit_form.h"
#include "ui/ui_helpers.h"

void DrawEditSession(EditSession* session){
  switch (session->state){
  case EDITOR:
    Draw_Map(&session->map, &session->camera);
    DrawSelectionOverlay(&session->buffer,&session->map,session->camera);
    DrawTilePalette(session);
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

    if (input->buttons_pressed & MOVEMENT_PRESSED) {
      float length = (input->dir.x != 0 && input->dir.y != 0) ? 0.707f : 1.0f;
      session->camera.target.x += input->dir.x * length * 400.0f * dt;
      session->camera.target.y += input->dir.y * length * 400.0f * dt;
    }
  }

  void DrawTilePalette(EditSession* session) {
      int startX = 20;
      int startY = SCREEN_HEIGHT - 60;

      for (int i = 0; i < 6; i++) {
          Rectangle slot = { startX + (i * 50), startY, 40, 40 };
          DrawRectangleRec(slot, TILE_REGISTRY[i].color);
      }
  }
