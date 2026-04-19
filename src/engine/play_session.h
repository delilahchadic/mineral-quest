#ifndef PlAY_SESSION_H
#define PlAY_SESSION_H

#include "systems/player.h"
#include "environment/map.h"
#include "environment/map_loader.h"
#include "ui/menu.h"
#include "systems/input.h"
#include "systems/physics.h"

typedef enum PlayState { ADVENTURE = 0, INVENTORY } PlayState;

typedef struct PlaySession{
  Player player;
  Map map;
  DialogManager manager;
  PlayState state;
  Menu menu;
} PlaySession;


void InitPlaySession(PlaySession* session);
void UpdatePlaySession(PlaySession* session);
void DrawPlaySession(PlaySession* session);
void DrawInventory(Menu* menu);
void UpdateInventory(PlaySession* session, Input* input);
void InitDialog(Map* map, DialogManager* manager);
#endif