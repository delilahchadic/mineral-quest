#ifndef REGISTER_H
#define REGISTER_H
#include "raylib.h"
#include "systems/character.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"
// 1. The Schemas (The "Blueprints")




// 2. The Stores (The "Warehouses")
extern ItemDefinition ITEM_REGISTRY[100];
extern Dialog DIALOG_REGISTRY[500];
extern Character CHARACTER_REGISTRY[200];
// 3. The Master Functions
void LoadItemRegistry();
void LoadDialogRegistry();
void LoadCharacterRegistry();

void ParseItemRow(char* line);
void ParseDialogRow(char* line);
void ParseCharacterRow(char* line);
#endif