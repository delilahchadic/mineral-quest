#ifndef WEAPONS_MENU_H
#define WEAPONS_MENU_H

#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "defs/constants.h"
#include "engine/palette.h"
#include "systems/weapon_grid.h"
#include "registry/register.h"
#include "registry/weapon_register.h"
#include "play/play_session.h"
#include "raylib.h"
#include <stdio.h>



// Function declarations
void UpdateWeaponsMenu(PlaySession *session, Input *input);
void DrawWeaponsMenu(PlaySession *session);

#endif
