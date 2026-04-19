#ifndef PlAY_SESSION_H
#define PlAY_SESSION_H

#include <stdio.h>
#include "systems/player.h"
#include "environment/map.h"
#include "environment/map_loader.h"
#include "ui/menu.h"
#include "systems/input.h"
#include "systems/physics.h"

typedef enum PlayState { ADVENTURE = 0, INVENTORY, TALKING, ITEM } PlayState;

typedef struct PlaySession{
  Player player;
  Map map;
  ScriptManager manager;
  PlayState state;
  Menu menu;
  char pendingItemName[100];
} PlaySession;


void InitPlaySession(PlaySession* session);
void UpdatePlaySession(PlaySession* session);
void DrawPlaySession(PlaySession* session);
void DrawInventory(Menu* menu);
void UpdateInventory(PlaySession* session, Input* input);
void InitDialog(Map* map, ScriptManager* manager);
int PollChest(Player* player ,Map* map);
#endif