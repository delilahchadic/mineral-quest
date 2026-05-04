#include "registry/command_register.h"

#include "defs/types_engine.h"
#include "engine/play_session.h"
#include "editor/edit_session.h"

Command COMMAND_REGISTRY[100] = {0};

void(*callbacks[100]) (void*) = {
  [0] = AdventureMode,
  [1] = EditMode,
  [3] = EditModeNew,
  [4] = EditModeLoad
};

// Used in Main menu to set the gsme state to bein game
void AdventureMode(void* context){
  Gamestate* gamestate = (Gamestate*)context;
  gamestate->session = calloc(1, sizeof(PlaySession));
  InitPlaySession(gamestate->session);
  gamestate->screen = GAMEPLAY;
}

void EditMode(void* context){
  Gamestate* gamestate = (Gamestate*)context;
  gamestate->edit_session = calloc(1, sizeof(EditSession));
  InitEditSession(gamestate->edit_session);
  gamestate->screen = EDIT_SCREEN;
}

void EditModeNew(void* context){
  EditSession* session = (EditSession*)context;
  session->state = EDITOR_PROMPT;
}

void EditModeLoad(void* context){
  EditSession* session = (EditSession*)context;
  session->state = LOAD_PROMPT;
}
