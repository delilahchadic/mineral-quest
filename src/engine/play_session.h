#ifndef PlAY_SESSION_H
#define PlAY_SESSION_H
#include "systems/player.h"
#include "environment/map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

typedef enum PlayState { ADVENTURE = 0, INVENTORY } PlayState;

typedef struct PlaySession{
  Player player;
  Map map;
  DialogManager manager;
  PlayState state;
} PlaySession;


void InitPlaySession(PlaySession* session);
void UpdatePlaySession(PlaySession* session);
void DrawPlaySession(PlaySession* session);
void DrawInventory(PlaySession* session);
void UpdateInventory(PlaySession* session);
#endif