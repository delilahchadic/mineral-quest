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
#include "engine/edit_session.h"
#include "registry/register.h"
#include "systems/input.h"
#include "ui/menu.h"
#include "defs/types_engine.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

void InitGame(Gamestate* gamestate);
void UpdateScene(Gamestate *gamestate);
void DrawScreen(Gamestate *gamestate);
void CloseGame(Gamestate *gamestate);

#endif