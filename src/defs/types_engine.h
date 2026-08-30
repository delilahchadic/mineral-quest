
#ifndef TYPES_ENGINE
#define TYPES_ENGINE

#include "defs/types_ui.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_core.h"
#include "defs/types_systems.h"
#include "raylib.h"

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY,MENU, EDIT_SCREEN } GameScreen;
typedef enum PlayState { ADVENTURE = 0, INVENTORY, TALKING, ITEM, MINERAL_INVENTORY , LEVEL_INVENTORY} PlayState;
typedef enum EditState {EDITOR_MENU = 0, EDITOR, EDITOR_PROMPT, LOAD_PROMPT}EditState;

typedef struct PlaySession{
  Player player;
  Map map;
  ScriptManager manager;
  PlayState state;
  Menu menu;
  char pendingItemName[100];
  Camera2D camera;
  Sound mineral_sound;
} PlaySession;

typedef struct EditLoader{
    char names[20][64];
    Rectangle buttons[20];
    int file_count;
    int current_file;
}EditLoader;

typedef struct EditSession{
  Map map;
  Camera2D camera;
  SystemMenu menu;
  EditorForm form;
  EditState state;
  TileEditor editor;
  Rectangle current_tile_panel;
  SelectionBuffer buffer;
  Vector2 last_selected_tile;
  Vector2 dragStart;
  bool isDragging;
  EditLoader loaded_files;
} EditSession;

typedef struct Gamestate{
  GameScreen screen;
  int framesCounter;
  SystemMenu main_menu;
  EditSession* edit_session;
  PlaySession* session;
} Gamestate;

typedef struct Command{
  char label[32];
  char description[100];
  void(*callback) (void* context);
} Command;


#endif
