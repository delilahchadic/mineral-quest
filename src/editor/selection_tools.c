#include "editor/selection_tools.h"

#include "raymath.h"
#include "core/selection_buffer.h"
#include "environment/map.h"

void UpdateSelectionBuffer(EditSession* session,Input *input){
    if((input->buttons_pressed & LEFT_MOUSE_CLICKED)){
        Vector2 world_mouse = GetScreenToWorld2D(input->mouse, session->camera);
        Vector2 selection = GetIsoWorldToGridWithHeight(&session->map,world_mouse);

        if(input->buttons_pressed & CONTROL_PRESSED){
            SetTileSelected(&session->buffer, selection.x, selection.y, !IsTileSelected(&session->buffer, selection.x, selection.y));
        }else if(input->buttons_pressed & SHIFT_PRESSED && session->last_selected_tile.x >= 0){
            if(IsTileSelected(&session->buffer, session->last_selected_tile.x, session->last_selected_tile.y)){
                SelectGridLine(session, selection );
            }
        }else{
            ClearSelection(&session->buffer);
            SetTileSelected(&session->buffer, selection.x, selection.y, !IsTileSelected(&session->buffer, selection.x, selection.y));
        }

        session->last_selected_tile = selection;
        session->dragStart = input->mouse;
    }

    if(input->buttons_pressed & LEFT_MOUSE_DOWN){
        Vector2 world_mouse = GetScreenToWorld2D(input->mouse, session->camera);
        Vector2 selection = GetIsoWorldToGridWithHeight(&session->map,world_mouse);
        if(Vector2Distance(session->dragStart, input->mouse) > DRAG_THRESHOLD)
            session->isDragging = true;
        if(session->isDragging){
            if(!(input->buttons_pressed & CONTROL_PRESSED)){
                ClearSelection(&session->buffer);
            }

            if(input->buttons_pressed & SHIFT_PRESSED){
                SelectRectangle(session, selection,true);
            }else{
                SelectRectangle(session, selection, false);
            }
        }
    }

    if(session->isDragging && input->buttons_pressed & LEFT_MOUSE_RELEASED){
        Vector2 world_mouse = GetScreenToWorld2D(input->mouse, session->camera);
        Vector2 selection = GetIsoWorldToGridWithHeight(&session->map,world_mouse);
        if(! (input->buttons_pressed & CONTROL_PRESSED)){
            ClearSelection(&session->buffer);
        }
        if(input->buttons_pressed & SHIFT_PRESSED){
            SelectRectangle(session, selection,true);
        }else{
            SelectRectangle(session, selection,false);
        }
        session->isDragging = false;
        session->last_selected_tile = selection;
        session->dragStart = input->mouse;
    }

  }

  void SelectGridLine(EditSession* session, Vector2 curr) {
      int x0 = session->last_selected_tile.x, y0 = session->last_selected_tile.y;
      int x1 = curr.x, y1 = curr.y;
      int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
      int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
      int err = dx + dy, e2;

      while (true) {
          // Replace this with your actual tile drawing or
          // logic to add to your selection buffer
          // DrawRectangle(x0 * TILE_SIZE, y0 * TILE_SIZE, TILE_SIZE, TILE_SIZE, WHITE);
          SetTileSelected(&session->buffer, x0, y0, true);
          if (x0 == x1 && y0 == y1) break;
          e2 = 2 * err;
          if (e2 >= dy) { err += dy; x0 += sx; }
          if (e2 <= dx) { err += dx; y0 += sy; }
      }
  }

  void SelectRectangle(EditSession* session, Vector2 curr, bool fill) {
      int x0 = session->last_selected_tile.x, y0 = session->last_selected_tile.y;
      int x1 = curr.x, y1 = curr.y;
      int minX = (x0 < x1) ? x0 : x1;
      int maxX = (x0 < x1) ? x1 : x0;
      int minY = (y0 < y1) ? y0 : y1;
      int maxY = (y0 < y1) ? y1 : y0;

      // Simple nested loop to fill the buffer
      for (int y = minY; y <= maxY; y++) {
          for (int x = minX; x <= maxX; x++) {
              if(!fill){
                  bool isEdge = (x == minX || x == maxX || y == minY || y == maxY);
                  if(isEdge){
                      SetTileSelected(&session->buffer, x, y, true);
                  }else{
                      SetTileSelected(&session->buffer, x, y, false);
                  }
              }else {
                  SetTileSelected(&session->buffer, x, y, true);
              }
          }
      }
  }
