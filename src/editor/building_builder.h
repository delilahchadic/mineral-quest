#ifndef BUILDING_BUILDER_H
#define BUILDING_BUILDER_H


#include "defs/types_engine.h"
#include "defs/types_env.h"
#include "defs/types_core.h"
#include "defs/types_ui.h"

void InitBuildingBuilder(BuildingBuilder* builder, int start_x, int start_y);
void DrawBuildingBuilder(BuildingBuilder* builder);
void UpdateBuildingBuilder(BuildingBuilder* builder, Map* map,Input* input, int last_x, int last_y);
#endif
