#ifndef EDIT_SESSION_H
#define EDIT_SESSION_H
#include "raylib.h"
#include "defs/types_engine.h"
#include "core/selection_buffer.h"
#include "defs/types_systems.h"

void InitEditSession(Gamestate* gamestate);
bool UpdateEditSession(Gamestate *gamestate, Input *input) ;
void CloseEditor(Gamestate *gamestate);
void InitEditSessionMouseTracking(EditSession* session);
void SetSelectionTileType(EditSession* session);
void AdjustSelectionHeight(EditSession* session);
#endif
