#ifndef REGISTER_H
#define REGISTER_H
#include "raylib.h"
#include "common.h"
#include "systems/player.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"

// 1. The Schemas (The "Blueprints")




// 2. The Stores (The "Warehouses")
extern Plant PLANT_REGISTRY[100];
extern ItemDefinition ITEM_REGISTRY[100];
extern Dialog DIALOG_REGISTRY[500];
extern Character CHARACTER_REGISTRY[200];
extern Player* PLAYER;
extern TileDefinition TILE_REGISTRY[6];
extern Texture2D SPRITE_OVERRIDE[10];

// 3. The Master Functions
void LoadItemRegistry();
void LoadDialogRegistry();
void LoadCharacterRegistry();
void LoadPlantRegistry();
void LoadSpriteOverrideRegistry();

void ParseItemRow(char* line);
void ParseDialogRow(char* line);
void ParseCharacterRow(char* line);
void ParsePlantRow(char* line);
void ParseSpriteOverrideRow(char* line);

void CloseCharacterRegistry();
void InitRegistries();

char* GetName(EntityType type, int id);
Texture2D* GetSprite(EntityType type, int id);
Texture2D* GetSpriteOverride(int id);
int GetDialogID(EntityType type, int id);
uint32_t GetDefaultTraitFlags(EntityType type, int id);
#endif