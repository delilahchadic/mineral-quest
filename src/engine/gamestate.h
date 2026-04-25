#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include "systems/player.h"
#include "environment/map.h"
#include <math.h>
#include <stdio.h>
#include "palette.h"
#include "engine/play_session.h"
#include "editor/edit_session.h"
#include "registry/register.h"
#include "systems/input.h"
#include "ui/menu.h"
#include "defs/types_engine.h"
#include "defs/constants.h"

void InitGame(Gamestate* gamestate);
void UpdateScene(Gamestate *gamestate);
void DrawScreen(Gamestate *gamestate);
void CloseGame(Gamestate *gamestate);

#endif
