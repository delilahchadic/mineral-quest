#ifndef MENU_H
#define MENU_H

#define MAX_SLOTS 20


#include "engine/common.h"
#include "string.h"
#include "ui/menu.h"
#include "engine/palette.h"
#include "engine/register.h"
#include "systems/input.h"

typedef struct Menu{
  int itemIds[MAX_SLOTS];
  int count;
  int selected;
  EntityType type;
} Menu;

void FillMenu(Menu* menu, int(*list)[MAX_SLOTS], int count);
void DrawMenu(Menu* menu);
bool UpdateMenu(Menu* menu, Input* input);
#endif