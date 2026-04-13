#ifndef PlAY_SESSION_H
#define PlAY_SESSION_H
#include "systems/player.h"
#include "environment/map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

typedef enum Play_State { ADVENTURE = 0, INVENTORY } Play_State;

typedef struct Play_Session{
  Player player;
  Map map;
  Dialog_Manager manager;
  Play_State state;
} Play_Session;


void InitPlaySession(Play_Session* session);
void UpdatePlaySession(Play_Session* session);
void DrawPlaySession(Play_Session* session);
void DrawInventory(Play_Session* session);
void UpdateInventory(Play_Session* session);
#endif