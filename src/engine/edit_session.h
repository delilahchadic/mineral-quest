#ifndef EDIT_SESSION_H
#define EDIT_SESSION_H
#include "raylib.h"
#include "defs/types_engine.h"
#include "engine/palette.h"
#include "core/selection_buffer.h"
#include "defs/types_systems.h"
#include "environment/map.h"
#include "registry/command_register.h"
#include "engine/palette.h"
#include "ui/dialog_box.h"
#include "defs/types_core.h"

void InitEditSession(EditSession* session);
bool UpdateEditSession(EditSession* session, Input* input);
void UpdateEditForm(EditSession* session, Input* input);
void DrawEditSession(EditSession* session);
void DrawEditForm(EditSession* session);
void CloseEditor(EditSession* session);
void UpdateEditorCamera(EditSession* session,Input *input);
void DrawSelectionOverlay(EditSession* session);
void UpdateSelectionBuffer(EditSession* session,Input *input);
void ProcessMapName(EditSession* session);
void ProcessMapWidth(EditSession* session);
void ProcessMapHeight(EditSession* session);
#endif
