#include "systems/player.h"

Player Get_Default_Player(){
  Player player = {0};
  player.position = (Vector2){ 0,0 };
  player.speed = 250.0f;
  Image i = LoadImage("sprite.png");
  player.sprite = LoadTextureFromImage(i);
  InitInventory(&player.inventory);
  player.inventory.itemIds[0]=1;
  player.inventory.itemIds[1]=2;
  player.inventory.itemIds[2]=3;
  player.inventory.itemIds[3]=4;
  player.inventory.count = 4;
  UnloadImage(i);
  return player;
}



void Draw_Player(Player* player){
  // Image i = LoadImage("char.png");
  // DrawTexture(*sprite,player->position.x, player->position.y, WHITE);
  Rectangle r= {0,0,32,64};
  DrawTextureRec(player->sprite,r,player->position, WHITE );
  // DrawRectangle(player->position.x, player->position.y,20,20, MAGENTA);
}

void Close_Player(Player* player){
  UnloadTexture(player->sprite);
}