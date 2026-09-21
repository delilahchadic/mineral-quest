#ifndef TARGETING_H
#define TARGETING_H
#include "defs/types_env.h"

MapEntity *GetTargetEntity(Map *map, int target_id);
void UpdatePlayerTargets(Map *map, PlayerTargeting *targeting);
void CycleTarget(PlayerTargeting *targeting);
#endif
