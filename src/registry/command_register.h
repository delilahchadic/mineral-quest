#ifndef COMMAND_REGISTER_H
#define COMMAND_REGISTER_H

#include <assert.h>
#include "defs/types_engine.h"


extern Command COMMAND_REGISTRY[100];
void(*callbacks[100]) (void* context);

//System Menu callbacks
void AdventureMode(void* context);
void EditMode(void* context);
void EditModeNew(void* context);
void EditModeLoad(void* context);


#endif
