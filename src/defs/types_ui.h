#ifndef TYPES_UI
#define TYPES_UI

#include "defs/types_entities.h"
#include "defs/types_systems.h"

typedef enum EditFormActiveField {FIELD_NAME = 0, FIELD_WIDTH, FIELD_HEIGHT, FIELD_CONFIRM}EditFormActiveField;
typedef enum TileEditorActiveTool{TILE_PALETTE = 0, HEIGHT_ADJUSTER, ENTITY_DRAWER}TileEditorActiveTool;
typedef enum UIResponse{UI_ACTION_NONE,UI_ACTION_CLICK, UI_ACTION_EXECUTE}UIResponse;
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
  char width_str[3];
  int width_count;
  char height_str[3];
  int height_count;
  EditFormActiveField active_field;
}EditorForm;

typedef struct TileEditor{
    TileEditorActiveTool tool;
    int selected_tile_type;
    EntityType selected_entity_type;
    int selected_entity_id;
    int height_delta;
}TileEditor;


void FillMenu(Menu* menu, int(*list)[MAX_SLOTS], int count);
void DrawMenu(Menu* menu);
bool UpdateMenu(Menu* menu, Input* input);

void FillSystemMenu(SystemMenu* menu, int ids[], int count, char* name);
void DrawSystemMenu(SystemMenu* menu);
int UpdateSystemMenu(SystemMenu* menu, Input* input);
#endif
