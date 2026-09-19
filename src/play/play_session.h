#ifndef PlAY_SESSION_H
#define PlAY_SESSION_H

#include "defs/types_engine.h"

void InitPlaySession(Gamestate* gamestate);
void UpdatePlaySession(Gamestate* gamestate);
void RebindItemMenu(PlaySession* session);
#endif
