#ifndef EDIT_LOADER_H
#define EDIT_LOADER_H

#include <stdbool.h>
#include "defs/types_engine.h"

void LoadFiles(EditSession* session);
bool UpdateLoadPrompt(EditSession* session,Input* input);
void DrawLoadForm(EditSession* session);
#endif
