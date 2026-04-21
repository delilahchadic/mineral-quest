#ifndef COMMAND_REGISTER_H
#define COMMAND_REGISTER_H

#include <assert.h>

typedef struct Command{
  char label[32];
  char description[100];
  void(*callback) (void* context);
} Command;

extern Command COMMAND_REGISTRY[100]; 
void(*callbacks[100]) (void* context);

void AdventureMode(void* context);
char* GetCommandLabel(int id);
void ExecuteCommand(int id, void* context);
char* GetCommandDescription(int id);
#endif