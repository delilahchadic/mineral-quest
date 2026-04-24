
#ifndef TYPES_ENGINE
#define TYPES_ENGINE

#include "defs/types_ui.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_core.h"
#include "defs/types_systems.h"
#include "raylib.h"

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY,MENU, EDIT_SCREEN } GameScreen;

typedef enum PlayState { ADVENTURE = 0, INVENTORY, TALKING, ITEM } PlayState;
typedef enum EditState {EDITOR_MENU = 0, EDITOR, EDITOR_PROMPT}EditState;

typedef struct PlaySession{
  Player player;
  Map map;
  ScriptManager manager;
  PlayState state;
  Menu menu;
  char pendingItemName[100];
  Camera2D camera;
} PlaySession;

typedef struct EditSession{
  Map map;
  Camera2D camera;
  SystemMenu menu;
  EditorForm form;
  EditState state;
  SelectionBuffer buffer;
  Vector2 last_selected_tile;
} EditSession;

typedef struct Gamestate{
  GameScreen screen;
  int framesCounter;
  SystemMenu main_menu;
  EditSession edit_session;
  PlaySession session;
} Gamestate;

#endif
