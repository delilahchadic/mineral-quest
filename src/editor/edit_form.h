#ifndef EDIT_FORM_H
#define EDIT_FORM_H

#include "defs/types_core.h"
#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "defs/types_ui.h"

void ProcessForm(EditorForm* form, Map* map, SelectionBuffer* buffer);
bool UpdateEditForm(EditorForm* form, Map* map, SelectionBuffer* buffer, Input* input);
void DrawEditForm(EditorForm* form);

#endif
