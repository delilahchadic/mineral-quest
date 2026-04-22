#ifndef TYPES_UI
#define TYPES_UI
#include "engine/common.h"
#include "defs/types_systems.h"
#define MAX_SLOTS 20

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

typedef struct EditorForm{
  char name[32];
  int letterCount;
  char width_str[5];
  int width_count;
  char height_str[5];
  int height_count;
  int active_field;
}EditorForm;

void FillMenu(Menu* menu, int(*list)[MAX_SLOTS], int count);
void DrawMenu(Menu* menu);
bool UpdateMenu(Menu* menu, Input* input);

void FillSystemMenu(SystemMenu* menu, int ids[], int count, char* name);
void DrawSystemMenu(SystemMenu* menu);
int UpdateSystemMenu(SystemMenu* menu, Input* input);
#endif