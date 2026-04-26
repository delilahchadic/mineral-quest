#ifndef TILE_EDITOR_H
#define TILE_EDITOR_H

#include "defs/types_ui.h"
#include "defs/types_systems.h"

void DrawTileEditor(TileEditor* tile_editor);
bool UpdateTilePalette(TileEditor* tile_editor, Input* input);
bool UpdateHeightAdjuster(TileEditor* tile_editor, Input* input);
void UpdateEntityDrawer(TileEditor* tile_editor, Input* input);
UIResponse UpdateTileEditor(TileEditor* tile_editor, Input* input);
void DrawTilePalette(TileEditor* editor);
void DrawHeightAdjuster();
void DrawEntityDrawer();
#endif
