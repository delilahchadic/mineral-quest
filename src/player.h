#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "world.h"
#include "inventory.h"
#include <stddef.h>

typedef struct player
{
  /* data */
  Vector2 position; // x and y coordinates
  float speed;      // How fast we move
  Texture2D sprite; // The visual representation
  Rectangle frameRec; // Which part of the sprite sheet to show (for animation)
  int facing;
  Inventory inventory;
} Player;

Player Get_Default_Player();
void Handle_Input(Player* player, World* world);
void Draw_Player(Player* player);
void Close_Player(Player* player);


#endif