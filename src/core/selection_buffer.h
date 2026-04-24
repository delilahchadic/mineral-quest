#ifndef SELECTION_BUFFER_H
#define SELECTION_BUFFER_H

#include "defs/types_core.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

void InitSelection(SelectionBuffer* sb, int w, int h);
void SetTileSelected(SelectionBuffer* sb, int x, int y, bool selected);
bool IsTileSelected(SelectionBuffer* sb, int x, int y);
void CloseSelection(SelectionBuffer* sb);
void ClearSelection(SelectionBuffer* sb);

#endif
