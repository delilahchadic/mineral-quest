#include "edit_session.h"
#include "core/selection_buffer.h"
#include "defs/types_core.h"
#include "engine/palette.h"
#include "environment/map.h"

void InitEditSession(EditSession* session){
  session->menu = (SystemMenu){0};
  session->map = (Map){0};
  session->form = (EditorForm){0};
  session->buffer = (SelectionBuffer){0};
  session->form.active_field = 0;
  FillSystemMenu(&session->menu, (int[]){3,2},2, "Edit Mode");
  session->last_selected_tile.x = -1;
  session->last_selected_tile.y = -1;
  session->camera.target = (Vector2){0,0};
  session->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };// Center of the 800x450 screen
  session->camera.rotation = 0.0f;
  session->camera.zoom = 1.0f;
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
    UpdateSelectionBuffer(session,input);
    UpdateEditorCamera(session,input);
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
        int width = atoi(session->form.width_str);
        int height = atoi(session->form.height_str);
        InitNewMap(&session->map, session->form.name, width, height);
        InitSelection(&session->buffer, width, height);
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
    Draw_Map(&session->map, &session->camera);
    DrawSelectionOverlay(session);
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

  void UpdateEditorCamera(EditSession* session,Input *input){
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    if (input->buttons_pressed & MOVEMENT_PRESSED) {
      // This is the "proper" way to get 0.707 for diagonals
      float length = (input->dir.x != 0 && input->dir.y != 0) ? 0.707f : 1.0f;
      session->camera.target.x += input->dir.x * length * 400.0f * dt;
      session->camera.target.y += input->dir.y * length * 400.0f * dt;
    }
  }

  void DrawTileHighlight(Vector2 top, Color color) {
      float hw = TILE_SIZE / 1.0f;
      float hh = TILE_SIZE / 2.0f;

      Vector2 right  = { top.x + hw, top.y + hh };
      Vector2 bottom = { top.x,      top.y + TILE_SIZE };
      Vector2 left   = { top.x - hw, top.y + hh };

      // Draw the "Floor" of the highlight
      DrawTriangleFan((Vector2[]){ top, left, bottom, right }, 4, color);

      // DRAW A SECOND "HOT" CENTER (This is the cheap glow trick)
      // A tiny white triangle at the very top makes it look like a light source
      // DrawTriangle(top, (Vector2){top.x - 5, top.y + 5}, (Vector2){top.x + 5, top.y + 5}, Fade(WHITE, 0.5f));
  }
void DrawCoolHighlight(Vector2 top, Color baseColor) {
    float time = GetTime();
    // Faster, sharper pulse for a "humming" energy feel
    float pulse = (sinf(time * 10.0f) * 0.1f) + 0.9f;

    // We want the core to be bright, but the aura to be soft
    Color auraColor = Fade(baseColor, 0.2f * pulse);
    Color coreColor = Fade(baseColor, 0.5f * pulse);
    Color lineHighlight = Fade(WHITE, 0.8f * pulse);

    // 1. Draw the "Aura" - slightly larger than the tile
    // We fake this by drawing the diamond shifted 2 pixels out in every direction
    // or just drawing it twice with a small offset.
    DrawTileHighlight(top, auraColor);

    // 2. Draw the Core (This is your main color)
    DrawTileHighlight(top, coreColor);

    // 3. The "Light Edge" - This makes it look like glass or energy
    // Draw only the top-left and top-right lines in WHITE
    float hw = TILE_SIZE / 1.0f;
    float hh = TILE_SIZE / 2.0f;
    Vector2 right  = { top.x + hw, top.y + hh };
    Vector2 left   = { top.x - hw, top.y + hh };

    DrawLineEx(top, right, 2.0f, lineHighlight);
    DrawLineEx(top, left, 2.0f, lineHighlight);
}

  void DrawSelectionOverlay(EditSession* session) {
      BeginMode2D(session->camera);
      BeginBlendMode(BLEND_ADDITIVE);
      Vector2 tl_corner = GetScreenToWorld2D((Vector2){0,0}, session->camera);
      Vector2 tr_corner = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,0}, session->camera);
      Vector2 bl_corner = GetScreenToWorld2D((Vector2){0,SCREEN_HEIGHT}, session->camera);
      Vector2 br_corner = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,SCREEN_HEIGHT}, session->camera);

      Vector2 g1 = GetIsoWorldToGrid(tl_corner);
      Vector2 g2 = GetIsoWorldToGrid(tr_corner);
      Vector2 g3 = GetIsoWorldToGrid(bl_corner);
      Vector2 g4 = GetIsoWorldToGrid(br_corner);
      float min_x = fminf(fminf(g1.x, g2.x), fminf(g3.x, g4.x)) -2;
      float max_x = fmaxf(fmaxf(g1.x, g2.x), fmaxf(g3.x, g4.x))+2;

      float min_y = fminf(fminf(g1.y, g2.y), fminf(g3.y, g4.y))-2;
      float max_y = fmaxf(fmaxf(g1.y, g2.y), fmaxf(g3.y, g4.y))+2;

      min_x = min_x < 0 ? 0: min_x;
      min_y = min_y < 0 ? 0: min_y;
      max_x = max_x > session->map.columns ? session->map.columns: max_x;
      max_y = max_y > session->map.rows ? session->map.rows: max_y;
      // Use the same min_x, max_x logic from before
      for (int y = min_y; y < max_y; y++) {
          for (int x = min_x; x < max_x; x++) {

              // Only draw if the bit is 1
              if (IsTileSelected(&session->buffer, x, y)) {
                  Vector2 g1 = GetWorldToIso((Vector2){ x * TILE_SIZE, y * TILE_SIZE });

                              // 2. Account for the height (h) so it sits on top
                              float h = session->map.grid[y][x].height * 8.0f;
                              Vector2 top_corner = { g1.x, g1.y - h };

                              // 3. Draw the highlight using the TOP corner as the start
                              // Note: I modified this to ensure it matches your grid lines
                              DrawCoolHighlight(top_corner,COLOR_BEAVIS_SHIRT);
              }
          }
      }
      EndBlendMode();
    EndMode2D();
  }

  void CloseEditor(EditSession* session){
      Close_Map(&session->map);
      CloseSelection(&session->buffer);
  }

void UpdateSelectionBuffer(EditSession* session,Input *input){
    if((input->buttons_pressed & LEFT_MOUSE_CLICKED)){
        ClearSelection(&session->buffer);
        Vector2 world_mouse = GetScreenToWorld2D(input->mouse, session->camera);
        Vector2 selection = GetIsoWorldToGrid(world_mouse);
        SetTileSelected(&session->buffer, selection.x, selection.y, true);
        session->last_selected_tile = selection;
      }
  }
