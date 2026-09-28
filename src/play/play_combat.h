#ifndef PLAY_COMBAT_H
#define PLAY_COMBAT_H

#include "defs/types_env.h"
#include "defs/types_systems.h"

void InitCombat(Map *map);
void UpdateCombat(Map *map);
void UpdatePlayerCombatAnimation(MapEntity *player, float dt);
void ExecuteDirectionalAttack(Map *map, Input *input);
void ExecuteTargetedAttack(Map *map, Player *player);
#endif
