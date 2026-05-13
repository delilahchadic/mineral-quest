#ifndef PLAY_INTERACT_H
#define PLAY_INTERACT_H

#include "raylib.h"
#include "defs/types_engine.h"
#include "defs/types_systems.h"


void InitDialog(Map* map, ScriptManager* manager);
int PollChest(Player* player, Map* map);
bool CheckMineralPickup(Vector2 playerPos, Vector2 mineralPos, float radius);
void CheckForMineralCollision(PlaySession* session);
#endif
