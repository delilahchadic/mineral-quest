
#include "defs/types_engine.h"
#include "edit_session.h"
void InitEditSession(EditSession* session){
  session->menu = (SystemMenu){0};
  session->map = (Map){0};
  session->form = (EditorForm){0};
  session->form.active_field = 0;
  FillSystemMenu(&session->menu, (int[]){3,2},2, "Edit Mode");
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
    UpdateEditForm(session,input);
    break;
  case EDITOR:
    break;
  default:
    break;
  }
  return true; 
}

void UpdateEditForm(EditSession* session, Input* input){
  switch(session->form.active_field){
    case 0:
      ProcessMapName(session);
      break;
    case 1:
      ProcessMapWidth(session);
      break;
    case 2:
      ProcessMapHeight(session);
      break;
    case 3:
      if(input->buttons_pressed & ENTER_PRESSED){
        InitNewMap(&session->map, session->form.name, atoi(session->form.width_str), atoi(session->form.height_str));
        session->state = EDITOR;
      }
      if(input->buttons_pressed & BACKSPACE_PRESSED){
        session->form.active_field = 0;
      }
      break;

    default:
      break;
  }

  if(input->buttons_pressed & ENTER_PRESSED){
      session->form.active_field++;
    }
}

void ProcessMapName(EditSession* session){
  int key = GetCharPressed();

  // 2. While there are keys in the queue, add them to our string
  while (key > 0) {
    // Only allow "printable" characters and check for buffer space
    if ((key >= 32) && (key <= 125) && (session->form.letterCount < 32)) {
      session->form.name[session->form.letterCount] = (char)key;
      session->form.name[session->form.letterCount + 1] = '\0'; // Always keep it null-terminated
      session->form.letterCount++;
    }
    key = GetCharPressed(); // Check the next key in the queue
  }

  // 3. Handle Backspace (separate from GetCharPressed)
  if (IsKeyPressed(KEY_BACKSPACE) && session->form.letterCount > 0) {
      session->form.letterCount--;
      session->form.name[session->form.letterCount] = '\0';
  }
}

void ProcessMapWidth(EditSession* session){
  int key = GetCharPressed();

  // 2. While there are keys in the queue, add them to our string
  while (key > 0) {
    // Only allow "printable" characters and check for buffer space
    if ((key >= 48) && (key <= 57) && (session->form.width_count < 32)) {
      session->form.width_str[session->form.width_count] = (char)key;
      session->form.width_str[session->form.width_count + 1] = '\0'; // Always keep it null-terminated
      session->form.width_count++;
    }
    key = GetCharPressed(); // Check the next key in the queue
  }

  // 3. Handle Backspace (separate from GetCharPressed)
  if (IsKeyPressed(KEY_BACKSPACE) && session->form.width_count > 0) {
      session->form.width_count--;
      session->form.width_str[session->form.width_count] = '\0';
  }
}

void ProcessMapHeight(EditSession* session){
  int key = GetCharPressed();

  // 2. While there are keys in the queue, add them to our string
  while (key > 0) {
    // Only allow "printable" characters and check for buffer space
    if ((key >= 48) && (key <= 57) && (session->form.height_count < 32)) {
      session->form.height_str[session->form.height_count] = (char)key;
      session->form.height_str[session->form.height_count + 1] = '\0'; // Always keep it null-terminated
      session->form.height_count++;
    }
    key = GetCharPressed(); // Check the next key in the queue
  }

  // 3. Handle Backspace (separate from GetCharPressed)
  if (IsKeyPressed(KEY_BACKSPACE) && session->form.height_count > 0) {
      session->form.height_count--;
      session->form.height_str[session->form.height_count] = '\0';
  }
}

void DrawEditSession(EditSession* session){
  switch (session->state)
  {
  case EDITOR:
    Draw_Map(&session->map);
    break;
  case EDITOR_MENU:
    DrawSystemMenu(&session->menu);
    break;
  case EDITOR_PROMPT:
    DrawEditForm(session);
    break;
  default:
    break;
  }
  
}
void DrawEditForm(EditSession* session){
// 1. Background - The Aged Paper
  ClearBackground(COLOR_PULP_PAPER);

  // // Dynamic horizontal line based on screen width
  int margin = 50;
  int uiWidth = SCREEN_WIDTH - (margin * 2);
  
  // // 2. Title - The "Ink" look
  DrawText("New Map", margin, 40, 30, COLOR_DUSTY_ROSE);
  DrawRectangle(50, 80, uiWidth, 2, COLOR_SUNKEN_INK); // A simple line
  DrawText("Name:", 200, 180, 20, COLOR_CERULEAN);
  DrawText(session->form.name, 300, 180, 20, COLOR_CERULEAN);
  DrawText("Width:", 200, 210, 20, COLOR_CERULEAN);
  DrawText(session->form.width_str, 300, 210, 20, COLOR_CERULEAN);
  DrawText("Height:", 200, 240, 20, COLOR_CERULEAN);
  DrawText(session->form.height_str, 300, 240, 20, COLOR_CERULEAN);
  
  // 4. Description Box (Bottom)
  char* text;
  switch(session->form.active_field){
    case 0:
      text = "Enter a Map Name";
      break;
    case 1:
      text = "Enter a Width";
      break;
    case 2:
      text = "Enter a Height";
      break;
    case 3:
      DrawDialog("", "Confirm?");
      break;
    default:
      break; 
  }
  if(session->form.active_field !=3){
    int descBoxHeight = 100;
    int descBoxY = SCREEN_HEIGHT - descBoxHeight - margin;
    DrawRectangle(margin, descBoxY, uiWidth, descBoxHeight, Fade(COLOR_SUNKEN_INK, 0.1f));
    DrawText(text, margin+10, descBoxY +40, 18, COLOR_SUNKEN_INK);
  }

  }