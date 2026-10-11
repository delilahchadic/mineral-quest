#ifndef PLAY_COMBAT_H
#define PLAY_COMBAT_H

#include "defs/types_env.h"
#include "defs/types_systems.h"

void InitCombat(Map *map,CombatState* combat);
void UpdateCombat(Map *map, CombatState* player_combat_state);
void UpdatePlayerCombatAnimation(MapEntity *player, CombatState* combat,float dt);
void ExecuteDirectionalAttack(Map *map,CombatState *combat, Input *input);
void ExecuteTargetedAttack(Map *map, Player *player, CombatState* combat);

#endif
