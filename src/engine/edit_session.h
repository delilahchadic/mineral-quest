#ifndef EDIT_SESSION_H
#define EDIT_SESSION_H
#include "raylib.h"
#include "defs/types_engine.h"
#include "environment/map.h"
#include "registry/command_register.h"
#include "engine/palette.h"
#include "ui/dialog_box.h"
void InitEditSession(EditSession* session);
bool UpdateEditSession(EditSession* session, Input* input);
void UpdateEditForm(EditSession* session, Input* input);
void DrawEditSession(EditSession* session);
void DrawEditForm(EditSession* session);

void ProcessMapName(EditSession* session);
void ProcessMapWidth(EditSession* session);
void ProcessMapHeight(EditSession* session);
#endif