#ifndef SELECTION_TOOLS_H
#define SELECTION_TOOLS_H

#include "raylib.h"
#include "defs/types_engine.h"
#include "defs/types_systems.h"

void UpdateSelectionBuffer(EditSession* session,Input *input);
void SelectGridLine(EditSession* session, Vector2 curr);
void SelectRectangle(EditSession* session, Vector2 curr,bool fill);
#endif
