#include "systems/player.h"

Player Get_Default_Player(){
  Player player = {0};

  InitInventory(&player.inventory);
  player.inventory.itemIds[0]=1;
  player.inventory.itemIds[1]=3;
  player.inventory.itemIds[2]=4;
  player.inventory.count = 3;
  player.speed = 250.0f;
  player.sprite = LoadTexture("data/sprites/sprite.png");
  return player;
}

void Close_Player(Player* player){
  UnloadTexture(player->sprite);
}

void GiveItem(Player* player,int id){
  if(id < 0){
    return;
  }

  int currItem = player->inventory.count;
  player->inventory.itemIds[currItem++] = id;
  player->inventory.count = currItem;
}