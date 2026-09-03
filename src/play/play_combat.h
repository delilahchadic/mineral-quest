#ifndef PLAY_COMBAT_H
#define PLAY_COMBAT_H

#include "defs/types_env.h"

void InitCombat(Map* map);
void UpdateCombat(Map* map);
void UpdatePlayerCombatAnimation(MapEntity* player, float dt);
#endif
