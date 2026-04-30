#ifndef TILE_PALETTE_H
#define TILE_PALETTE_H


#include "defs/types_ui.h"
#include "defs/types_env.h"
#include "defs/types_core.h"

void InitTilePalette(TilePalette* palette, int start_x, int start_y);
void DrawTilePalette(TilePalette* palette);
void UpdateTilePalette(TilePalette* palette, Map* map, SelectionBuffer* buffer, Input* input);
#endif
