#include "systems/player.h"

Player Get_Default_Player(){
  Player player = {0};

  InitInventory(&player.inventory);
  player.inventory.itemIds[0]=1;
  player.inventory.itemIds[1]=2;
  player.inventory.itemIds[2]=3;
  player.inventory.itemIds[3]=4;
  player.inventory.count = 4;
  player.speed = 250.0f;
  // player.jumpOffset = 10.0f;
  player.sprite = LoadTexture("sprite.png");
  return player;
}

void Close_Player(Player* player){
  UnloadTexture(player->sprite);
}