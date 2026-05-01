#ifndef TYPES_UI
#define TYPES_UI

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "raylib.h"

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

typedef struct TilePalette{
    Rectangle tile_type_buttons[20];
    Rectangle execute_button;
    int selected_tile_type;
    Color colors[20];
} TilePalette;

typedef struct HeightAdjuster{
    Rectangle minus_5;
    Rectangle minus_1;
    Rectangle plus_1;
    Rectangle plus_5;
}HeightAdjuster;

typedef struct EntityDrawer{
    EntityType current_type;
    int current_page;
    int entities_per_page;
    Rectangle prev_page_button;
    Rectangle next_page_button;
    int selected_id;
    Rectangle buttons[10];
    Rectangle plant_button;
    Rectangle character_button;
    Rectangle item_button;
    int ids[10];
}EntityDrawer;

typedef struct TileEditor{
    Rectangle panel;
    TileEditorActiveTool tool;
    Rectangle save_button;
    Rectangle tile_palette_button;
    Rectangle height_adjuster_button;
    Rectangle entity_drawer_button;
    TilePalette palette;
    HeightAdjuster adjuster;
    EntityDrawer drawer;
}TileEditor;


void FillMenu(Menu* menu, int(*list)[MAX_SLOTS], int count);
void DrawMenu(Menu* menu);
bool UpdateMenu(Menu* menu, Input* input);

void FillSystemMenu(SystemMenu* menu, int ids[], int count, char* name);
void DrawSystemMenu(SystemMenu* menu);
int UpdateSystemMenu(SystemMenu* menu, Input* input);
#endif
