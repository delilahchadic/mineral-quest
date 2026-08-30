#ifndef PLAY_INVENTORY_H
#define PLAY_INVENTORY_H

#include "defs/types_engine.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"

void DrawInventory(Menu* menu);
void DrawMineralInventory(Player* player);
void DrawLevelInventory();
void UpdateInventory(PlaySession* session, Input* input);
void UpdateMineralInventory(PlaySession* session, Input* input);
void UpdateLevelInventory(PlaySession* session, Input* input);
#endif
