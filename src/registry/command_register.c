#include "registry/command_register.h"
#include "defs/types_engine.h"

Command COMMAND_REGISTRY[100] = {0};

void(*callbacks[100]) (void*) = {
  [0] = AdventureMode,
  [1] = EditMode,
  [3] = EditModeNew,
  [4] = EditModeLoad
};

// Used in Main menu to set the gsme state to bein game
void AdventureMode(void* context){
  Gamestate* state = (Gamestate*)context;
  state->screen = GAMEPLAY;
}

void EditMode(void* context){
  Gamestate* state = (Gamestate*)context;
  state->screen = EDIT_SCREEN;
}

void EditModeNew(void* context){
  EditSession* session = (EditSession*)context;
  session->state = EDITOR_PROMPT;
}

void EditModeLoad(void* context){
  EditSession* session = (EditSession*)context;
  session->state = LOAD_PROMPT;
}

char* GetCommandLabel(int id){
  return COMMAND_REGISTRY[id].label;
}

char* GetCommandDescription(int id){
  return COMMAND_REGISTRY[id].description;
}

void ExecuteCommand(int id, void* context){
  callbacks[id](context);
}
