#ifndef PlAY_SESSION_H
#define PlAY_SESSION_H

#include <stdio.h>
#include "systems/player.h"
#include "environment/map.h"
#include "environment/map_loader.h"
#include "ui/menu.h"
#include "systems/input.h"
#include "systems/physics.h"
#include "defs/types_engine.h"

void InitPlaySession(PlaySession* session);
void UpdatePlaySession(PlaySession* session);
void DrawPlaySession(PlaySession* session);
void DrawInventory(Menu* menu);
void UpdateInventory(PlaySession* session, Input* input);
void InitDialog(Map* map, ScriptManager* manager);
int PollChest(Player* player, Map* map);
void AdjustCamera(PlaySession* session, bool dialog);
bool CheckMineralPickup(Vector2 playerPos, Vector2 mineralPos, float radius);
#endif
