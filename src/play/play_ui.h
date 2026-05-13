#ifndef PLAY_UI_H
#define PLAY_UI_H
#include "defs/types_engine.h"
#include "ui/menu.h"
#include <stdbool.h>

void AdjustCamera(PlaySession* session, bool dialog, float dt);

void DrawPlaySession(PlaySession* session);
#endif
