#ifndef TILE_EDITOR_H
#define TILE_EDITOR_H

#include "defs/types_core.h"
#include "defs/types_ui.h"
#include "defs/types_systems.h"
#include "defs/types_env.h"
#include <stdbool.h>


void DrawEntityDrawer();
void DrawHeightAdjuster();
void DrawTileEditor(TileEditor* tile_editor);
void DrawTilePalette(TileEditor* editor);

bool UpdateTilePalette(TileEditor* tile_editor, Map* map, SelectionBuffer* buffer, Input* input);
bool UpdateHeightAdjuster(TileEditor* tile_editor, Map* map, SelectionBuffer* buffer, Input* input);
void UpdateEntityDrawer(TileEditor* tile_editor, Map* map, SelectionBuffer* buffer, Input* input);
bool UpdateTileEditor(TileEditor* tile_editor, Map* map, SelectionBuffer* buffer, Input* input);

#endif
