#ifndef REGISTER_H
#define REGISTER_H
#include "raylib.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "palette.h"
// 1. The Schemas (The "Blueprints")




// 2. The Stores (The "Warehouses")
extern ItemDefinition ITEM_REGISTRY[100];
extern Dialog DIALOG_REGISTRY[500];

// 3. The Master Functions
void LoadItemRegistry();
void LoadDialogRegistry();

void ParseItemRow(char* line);
void ParseDialogRow(char* line);
#endif