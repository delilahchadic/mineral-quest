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


typedef struct {
    int id;
    char text[256];
} DialogDef;

// 2. The Stores (The "Warehouses")
extern ItemDefinition ITEM_REGISTRY[100];
extern DialogDef DIALOG_REGISTRY[500];

// 3. The Master Functions
void LoadItemRegistry();
#endif