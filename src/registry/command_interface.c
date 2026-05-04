#include "registry/command_interface.h"


char* GetCommandLabel(int id){
  return COMMAND_REGISTRY[id].label;
}

char* GetCommandDescription(int id){
  return COMMAND_REGISTRY[id].description;
}

void ExecuteCommand(int id, void* context){
  callbacks[id](context);
}
