#ifndef MENU_H
#define MENU_H

#include "defs/constants.h"
#include "string.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "registry/command_register.h"
#include "systems/input.h"
#include "defs/types_ui.h"



void FillMenu(Menu* menu, int(*list)[MAX_SLOTS], int count);
void DrawMenu(Menu* menu);
bool UpdateMenu(Menu* menu, Input* input);

void FillSystemMenu(SystemMenu* menu, int ids[], int count, char* name);
void DrawSystemMenu(SystemMenu* menu);
int UpdateSystemMenu(SystemMenu* menu, Input* input);

#endif
