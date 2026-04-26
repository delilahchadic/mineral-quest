#ifndef EDIT_UI_H
#define EDIT_UI_H

#include <stdlib.h>

#include "defs/types_engine.h"
#include "defs/types_systems.h"
#include "defs/types_env.h"
#include "environment/map.h"
#include "defs/types_core.h"
#include "core/selection_buffer.h"

void DrawEditSession(EditSession* session);
void UpdateEditorCamera(EditSession* session,Input *input);

#endif
