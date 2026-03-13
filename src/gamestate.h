#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include "player.h"
#include "world.h"
#include <math.h>
#include "character.h"
#include "dialog.h"
#include <stdio.h>
#include "palette.h"

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY } GameScreen;
typedef struct Gamestate
{
  GameScreen screen;
  int framesCounter;
  Player player;
  World world;
  Character character;
  Dialog_Manager manager;
} Gamestate;

void InitGame(Gamestate* gamestate);
void UpdateGameplay(Gamestate *gamestate);
void UpdateScene(Gamestate *gamestate);
void DrawGameplay(Gamestate *gamestate);
void DrawScreen(Gamestate *gamestate);
void CloseGame(Gamestate *gamestate);
#endif