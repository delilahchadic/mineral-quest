#ifndef MENU_H
#define MENU_H


#define MAX_SLOTS 20

#include "engine/common.h"
#include "string.h"
#include "ui/menu.h"
#include "engine/palette.h"
#include "registry/register.h"
#include "registry/command_register.h"
#include "systems/input.h"


typedef struct SystemMenu{
  char name[32];
  int ids[MAX_SLOTS];
  int count;
  int selected;
}SystemMenu;

typedef struct Menu{
  int itemIds[MAX_SLOTS];
  int count;
  int selected;
  int functionId;
  EntityType type;
}Menu;

void FillMenu(Menu* menu, int(*list)[MAX_SLOTS], int count);
void DrawMenu(Menu* menu);
bool UpdateMenu(Menu* menu, Input* input);

void FillSystemMenu(SystemMenu* menu, int ids[], int count, char* name);
void DrawSystemMenu(SystemMenu* menu);
int UpdateSystemMenu(SystemMenu* menu, Input* input);
#endif