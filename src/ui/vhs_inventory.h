#ifndef VHS_MENU_H
#define VHS_MENU_H


#include "defs/types_entities.h"
#include "ui/menu.h"

void RebindVHSMenu(Menu* menu, Player* player);
void UpdateVHSInventory(PlaySession* session, Input* input);
void DrawVHSInventory(Menu* menu);
#endif
