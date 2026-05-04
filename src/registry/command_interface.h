#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <assert.h>
#include "registry/command_register.h"
#include "defs/types_engine.h"

char* GetCommandLabel(int id);
void ExecuteCommand(int id, void* context);
char* GetCommandDescription(int id);

#endif
