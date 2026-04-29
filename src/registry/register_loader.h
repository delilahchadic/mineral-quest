#ifndef REGISTER_LOADER_H
#define REGISTER_LOADER_H
#include "raylib.h"
#include "registry/register.h"
#include "registry/command_register.h"
#include <stdlib.h>
#include <stdio.h>

int LoadItemRegistry();
void LoadDialogRegistry();
void LoadCharacterRegistry();
void LoadPlantRegistry();
void LoadSpriteOverrideRegistry();

void LoadCommandRegistry();

void CloseRegistries();
void CloseCharacterRegistry();
void ClosePlantRegistry();
void CloseSpriteOverrideRegistry();

void ParseItemRow(char* line);
void ParseDialogRow(char* line);
void ParseCharacterRow(char* line);
void ParsePlantRow(char* line);
void ParseSpriteOverrideRow(char* line);
void ParseCommandRegistryRow(char* line);

void CloseCharacterRegistry();
void InitRegistries();
#endif
