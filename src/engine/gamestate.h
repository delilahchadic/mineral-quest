#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include "systems/player.h"
#include "environment/map.h"
#include <math.h>
#include "systems/script_manager.h"
#include <stdio.h>
#include "palette.h"
#include "engine/play_session.h"
#include "register.h"
#include "environment/map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY } GameScreen;
typedef struct Gamestate{
  GameScreen screen;
  int framesCounter;
  PlaySession session;
} Gamestate;

void InitGame(Gamestate* gamestate);
void UpdateScene(Gamestate *gamestate);
void DrawScreen(Gamestate *gamestate);
void CloseGame(Gamestate *gamestate);
#endif