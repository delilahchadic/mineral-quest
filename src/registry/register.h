#ifndef REGISTER_H
#define REGISTER_H
#include "raylib.h"

#include "defs/types_entities.h"
#include "defs/types_env.h"
#include "systems/player.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "engine/palette.h"

//entity register
extern Enemy ENEMY_REGISTRY[100];
extern Plant PLANT_REGISTRY[100];
extern Character CHARACTER_REGISTRY[200];
extern ItemDefinition ITEM_REGISTRY[100];
extern Portal PORTAL_REGISTRY[100];

//system register
extern Message DIALOG_REGISTRY[500];
extern Player GLOBAL_PLAYER;
extern Player* PLAYER;
extern TileDefinition TILE_REGISTRY[42];

//play register
extern int WORLD_COUNT;
extern World WORLD_REGISTER[100];
extern char* STATS_NAMES[STAT_COUNT];
extern ExchangeNode NODE_REGISTRY[100];
extern Exchange EXCHANGE_REGISTRY[1000];

//Media Register
extern Texture2D PORTAL_TV_SPRITE;
extern Texture2D PORTAL_CRYSTAL_SPRITE;
extern Texture2D RUSTY_HEADSHOT_SPRITE;
extern Texture2D SPRITE_OVERRIDE[10];
extern Sound MINERAL_SOUND;

// 3. Get Functions
char* GetName(EntityType type, int id);
char* GetNodeName(int node_id);
char* GetWorldName(int world_id);
char* GetDescription(EntityType type, int id);
Texture2D* GetSprite(EntityType type, int id);
Texture2D* GetSpriteOverride(int id);
int GetDialogID(EntityType type, int id);
uint32_t GetDefaultTraitFlags(EntityType type, int id);
StatBlock* GetStats(EntityType type, int id);
int GetEntityTypeCount(EntityType type);
void SetEntityTypeCount(EntityType type, int count);
int GetAccesorySlot(EntityType type, int id);
int GetCharacterCount();
int GetCharacterId(int node_id);
int GetWorldIDFromPortal(int portal_id);
Vector2 GetDestination(EntityType type, int id);
char* GetWorldNameFromPortalId(int portal_id);
PortalType GetPortalType(int id);
Vector2 GetEntityCenter(MapEntity* entity);
#endif
