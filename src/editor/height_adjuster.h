#ifndef HEIGHT_ADJUSTER_H
#define HEIGHT_ADJUSTER_H

#include "defs/types_engine.h"
#include "defs/types_env.h"
#include "defs/types_core.h"

void InitHeightAdjuster(HeightAdjuster* editor, int start_x, int start_y);
void DrawHeightAdjuster(HeightAdjuster* editor);
void UpdateHeightAdjuster(HeightAdjuster* tile_editor, Map* map, SelectionBuffer* buffer, Input* input);
#endif
