
#ifndef TYPES_ENGINE
#define TYPES_ENGINE

#include "defs/types_ui.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"

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
} PlaySession;

typedef struct EditSession{
  Map map;
  SystemMenu menu;
  EditorForm form;
  EditState state;
} EditSession;

typedef struct Gamestate{
  GameScreen screen;
  int framesCounter;
  SystemMenu main_menu;
  EditSession edit_session;
  PlaySession session;
} Gamestate;

#endif