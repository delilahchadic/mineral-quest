#ifndef EDIT_SESSION_H
#define EDIT_SESSION_H
#include "raylib.h"
#include "defs/types_engine.h"
#include "core/selection_buffer.h"
#include "defs/types_systems.h"

void InitEditSession(EditSession* session);
bool UpdateEditSession(EditSession* session, Input* input);
void CloseEditor(EditSession* session);
void InitEditSessionMouseTracking(EditSession* session);

#endif
