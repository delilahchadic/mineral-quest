#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "defs/types_engine.h"

void InitGame(Gamestate* gamestate);
void UpdateScene(Gamestate *gamestate);
void DrawScreen(Gamestate *gamestate);
void CloseGame(Gamestate *gamestate);

#endif
