#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include "systems/player.h"
#include "environment/map.h"
// #include "world.h"
#include <math.h>
#include "systems/character.h"
#include "systems/dialog.h"
#include <stdio.h>
#include "palette.h"
#include "register.h"
#include "environment/map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY,INVENTORY } GameScreen;
typedef struct Gamestate
{
  GameScreen screen;
  int framesCounter;
  Player player;
  Map map;
  Dialog_Manager manager;

} Gamestate;

void InitGame(Gamestate* gamestate);
void UpdateGameplay(Gamestate *gamestate);
void UpdateScene(Gamestate *gamestate);
void DrawGameplay(Gamestate *gamestate);
void DrawScreen(Gamestate *gamestate);
void CloseGame(Gamestate *gamestate);
void UpdateInventory(Gamestate* gamestate);
#endif