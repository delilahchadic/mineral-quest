#ifndef PLAY_INTERACT_H
#define PLAY_INTERACT_H

#include "raylib.h"
#include "defs/types_engine.h"
#include "defs/types_systems.h"


void InitDialog(Map* map, ScriptManager* manager);
char* GatherEntity(Player* player, Map* map);
void CheckAndCollectMinerals(Map* map);
void CheckHazards(Map *map, float dt);
#endif
