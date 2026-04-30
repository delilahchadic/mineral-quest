#include "editor/edit_form.h"

#include "core/selection_buffer.h"
#include "defs/types_engine.h"
#include "environment/map.h"
#include "ui/ui_helpers.h"

bool UpdateEditForm(EditorForm* form, Map* map, SelectionBuffer* buffer, Input* input){
    switch(form->active_field){
        case FIELD_NAME: // Name
            ProcessTextInput(form->name, &form->letterCount, 32, false);
            break;
        case FIELD_WIDTH: // Width
            ProcessTextInput(form->width_str, &form->width_count, 3, true);
            break;
        case FIELD_HEIGHT: // Height
            ProcessTextInput(form->height_str, &form->height_count, 3, true);
            break;
        case FIELD_CONFIRM: // Confirm
            if(input->buttons_pressed & ENTER_PRESSED){
                ProcessForm(form,map,buffer);
                return true;
            }
            if(input->buttons_pressed & BACKSPACE_PRESSED){
                form->active_field = FIELD_NAME;
            }
            break;
        default:
            break;
    }

    if(input->buttons_pressed & ENTER_PRESSED){
        if(form->active_field == FIELD_CONFIRM) form->active_field = FIELD_NAME;
        else form->active_field++;
    }
   return false;
}

void ProcessForm(EditorForm* form, Map* map, SelectionBuffer* buffer){
    int width = atoi(form->width_str);
    int height = atoi(form->height_str);
    InitNewMap(map, form->name, width, height);
    InitSelection(buffer, width, height);
}

void DrawEditForm(EditorForm* form){
// 1. Background - The Aged Paper
  ClearBackground(COLOR_PULP_PAPER);

  int margin = 50;
  int uiWidth = SCREEN_WIDTH - (margin * 2);

  // // 2. Title - The "Ink" look
  DrawText("New Map", margin, 40, 30, COLOR_DUSTY_ROSE);
  DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line
  DrawText("Name:", SCREEN_WIDTH/4, 180, 20, COLOR_CERULEAN_CORE);
  DrawText(form->name, SCREEN_WIDTH/2, 180, 20, COLOR_CERULEAN_CORE);
  DrawText("Width:", SCREEN_WIDTH/4, 210, 20, COLOR_CERULEAN_CORE);
  DrawText(form->width_str, SCREEN_WIDTH/2, 210, 20, COLOR_CERULEAN_CORE);
  DrawText("Height:", SCREEN_WIDTH/4, 240, 20, COLOR_CERULEAN_CORE);
  DrawText(form->height_str, SCREEN_WIDTH/2, 240, 20, COLOR_CERULEAN_CORE);

  // 4. Description Box (Bottom)
  char* text;
  switch(form->active_field){
    case FIELD_NAME:
      text = "Enter a Map Name";
      break;
    case FIELD_WIDTH:
      text = "Enter a Width";
      break;
    case FIELD_HEIGHT:
      text = "Enter a Height";
      break;
    case FIELD_CONFIRM:
      DrawDialog("", "Confirm?");
      break;
    default:
      break;
  }
  if(form->active_field != FIELD_CONFIRM){
    int descBoxHeight = 100;
    int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;
    DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));
    DrawText(text, margin+10, descBoxY +40, 18, COLOR_SUNKEN_INK);
  }

}
