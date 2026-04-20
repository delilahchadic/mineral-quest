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

extern Plant PLANT_REGISTRY[100];
extern Character CHARACTER_REGISTRY[200];
extern ItemDefinition ITEM_REGISTRY[100];
extern Message DIALOG_REGISTRY[500];

extern Player GLOBAL_PLAYER;
extern Player* PLAYER;
extern TileDefinition TILE_REGISTRY[6];
extern Texture2D SPRITE_OVERRIDE[10];

// 3. Get Functions
char* GetName(EntityType type, int id);
char* GetDescription(EntityType type, int id);
Texture2D* GetSprite(EntityType type, int id);
Texture2D* GetSpriteOverride(int id);
int GetDialogID(EntityType type, int id);
uint32_t GetDefaultTraitFlags(EntityType type, int id);
#endif