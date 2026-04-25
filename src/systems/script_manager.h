#ifndef SCRIPT_MANAGER_H
#define SCRIPT_MANAGER_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"
#include "registry/register.h"
#include "ui/dialog_box.h"
#include "defs/types_systems.h"

void InitScriptManager(ScriptManager* manager, int initialCapacity);
void DrawMessage(ScriptManager* manager);
void SetActiveMessage(ScriptManager* manager, int id);
void UpdateScriptManager(ScriptManager* manager, Input* input);

#endif
