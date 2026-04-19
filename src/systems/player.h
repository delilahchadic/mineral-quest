#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "systems/inventory.h"
#include <stddef.h>
typedef struct player
{
  /* data */
  Inventory inventory;
  float speed;   
  Texture2D sprite;     // How fast we move
} Player;

Player Get_Default_Player();
void Close_Player(Player* player);
void GiveItem(Player* player,int id);

#endif