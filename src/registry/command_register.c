#include "registry/command_register.h"
#include "engine/common.h"
Command COMMAND_REGISTRY[100] = {0};

void(*callbacks[100]) (void*) = {
  [0] = AdventureMode
};

// Used in Main menu to set the gsme state to bein game
void AdventureMode(void* context){
  Gamestate* state = (Gamestate*)context;
  state->screen = GAMEPLAY;
}

char* GetCommandLabel(int id){
  return COMMAND_REGISTRY[id].label;
}

char* GetCommandDescription(int id){
  return COMMAND_REGISTRY[id].description;
}

void ExecuteCommand(int id, void* context){
  // assert((id >= 0 && id < 100) && COMMAND_REGISTRY[id].callback != NULL && "invalid command id");
  callbacks[id](context);
}