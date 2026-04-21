
#ifndef TYPES_ENGINE
#define TYPES_ENGINE

#include "defs/types_ui.h"
#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY,MENU } GameScreen;

typedef enum PlayState { ADVENTURE = 0, INVENTORY, TALKING, ITEM } PlayState;

typedef struct PlaySession{
  Player player;
  Map map;
  ScriptManager manager;
  PlayState state;
  Menu menu;
  char pendingItemName[100];
} PlaySession;


typedef struct Gamestate{
  GameScreen screen;
  int framesCounter;
  SystemMenu main_menu;
  PlaySession session;
} Gamestate;

#endif