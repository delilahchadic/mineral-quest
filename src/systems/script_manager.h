#ifndef SCRIPT_MANAGER_H
#define SCRIPT_MANAGER_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"
#include "engine/common.h"
#include "engine/register.h"
#include "ui/dialog_box.h"
#include "systems/input.h"

typedef struct ScriptManager{
  Message* active_messsage;
  int count;
  int capacity;
  bool active;
  int currentID;
} ScriptManager;


void InitScriptManager(ScriptManager* manager, int initialCapacity);
void DrawMessage(ScriptManager* manager);
void SetActiveMessage(ScriptManager* manager, int id);
void UpdateScriptManager(ScriptManager* manager, Input* input);
#endif