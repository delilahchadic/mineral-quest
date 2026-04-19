#include "systems/script_manager.h"

void InitScriptManager(ScriptManager* manager, int initialCapacity){
  manager->count =0;
  manager->capacity = initialCapacity;
  manager->active =false;
  manager->currentID = -1;
  manager->active_messsage = NULL;
};


void SetActiveMessage(ScriptManager* manager,int dialogId){
  manager->active_messsage = &DIALOG_REGISTRY[dialogId];
}

void DrawMessage(ScriptManager* manager){

  if(!manager->active || manager->active_messsage == NULL){
    return;
  }
  DrawDialog(manager->active_messsage->character_name, manager->active_messsage->text);
  
}

void UpdateScriptManager(ScriptManager* manager, Input* input){
  if (manager->active && (input->buttons_pressed & INTERACT_PRESSED)) {
    // For now, let's just close it. 
    // Later, this will call Advance_Dialogue to check for nextId
    if(manager->active_messsage->nextid != -1){
      SetActiveMessage(manager, manager->active_messsage->nextid);
      return;
    }
    manager->active = false;
    manager->active_messsage = NULL; 
  }
}