#ifndef ENTITY_DRAWER_H
#define ENTITY_DRAWER_H

#include "raylib.h"

#include "defs/types_ui.h"
#include "defs/types_env.h"
#include "defs/types_core.h"
#include "defs/types_systems.h"

void InitEnitityDrawer(EntityDrawer* drawer, int startX, int startY);
bool UpdateEnitityDrawer(EntityDrawer* drawer, Map* map, Input* input,Camera2D* camera);
void DrawEnitityDrawer(EntityDrawer* drawer);

#endif
