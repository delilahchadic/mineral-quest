#ifndef TYPES_UI
#define TYPES_UI

#include "defs/constants.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "raylib.h"

typedef enum EditFormActiveField {FIELD_NAME = 0, FIELD_WIDTH, FIELD_HEIGHT, FIELD_CONFIRM}EditFormActiveField;
typedef enum TileEditorActiveTool{TILE_PALETTE = 0, HEIGHT_ADJUSTER, ENTITY_DRAWER, BUIDING_BUILDER}TileEditorActiveTool;
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

typedef struct EquipMenu{
    int itemIds[MAX_SLOTS];
    int count;
    EquipSlot mode;
    int activeSlot; // used to track which slot is being chosen weapon acc1, acc2, etc
    int activeItemSlot; // used to track which item is selected zo once we are look ing at the weapons this selects the active one
} EquipMenu;

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
    Rectangle tile_type_buttons[28];
    Rectangle execute_button;
    int selected_tile_type;
    Color colors[28];
} TilePalette;

typedef struct HeightAdjuster{
    Rectangle minus_5;
    Rectangle minus_1;
    Rectangle plus_1;
    Rectangle plus_5;
    Rectangle minus_25;
    Rectangle minus_10;
    Rectangle plus_10;
    Rectangle plus_25;
    Rectangle minus_8;
    Rectangle plus_8;
    Rectangle zero;
    Rectangle twenty_five;
    Rectangle fifty;
    Rectangle one_hundred;
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
    Rectangle mineral_button;
    Rectangle item_button;
    Rectangle enemy_button;
    Rectangle portal_button;
    int ids[9];
}EntityDrawer;

typedef struct BuildingBuilder{
    BuildingZone* current_building;
    int last_x, last_y;
    int x1, y1;
    int x2, y2;
    int curr_height;
    int min_height, max_height;
    Rectangle add_building_button;
    Rectangle minus_5;
    Rectangle minus_1;
    Rectangle plus_1;
    Rectangle plus_5;
    Rectangle set_tile_1;
    Rectangle set_tile_2;
    Rectangle set_min, set_max;
    Rectangle set_min_height;
    Rectangle set_max_height;
    Rectangle cycle_selected_building;
    Rectangle remove_building;
}BuildingBuilder;

typedef struct TileEditor{
    Rectangle panel;
    TileEditorActiveTool tool;
    Rectangle save_button;
    Rectangle tile_palette_button;
    Rectangle height_adjuster_button;
    Rectangle entity_drawer_button;
    Rectangle building_builder_button;
    TilePalette palette;
    HeightAdjuster adjuster;
    EntityDrawer drawer;
    BuildingBuilder builder;
}TileEditor;

#endif
