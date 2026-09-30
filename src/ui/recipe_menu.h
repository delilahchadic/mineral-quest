#ifndef RECIPE_INVENTORY_H
#define RECIPE_INVENTORY_H

#include "defs/types_engine.h"
#include "defs/types_systems.h"

void DrawRecipeInventory(PlaySession *session);
void UpdateRecipeInventory(PlaySession *session, Input *input);
void RebindRecipeMenu(Menu *menu, Player *player);

#endif
