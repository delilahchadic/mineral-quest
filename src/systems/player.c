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

void Handle_Input(Player* player, Map* map){
  float dt = GetFrameTime();
  // float speed = player->speed;
  Vector2 dir = { 0, 0 };

  if(IsKeyDown(KEY_W)){
    dir.y -=1;
  }
  if(IsKeyDown(KEY_A)){
    dir.x -=1;

  }
   if(IsKeyDown(KEY_D)){
    dir.x +=1;
  }

   if(IsKeyDown(KEY_S)){
    dir.y +=1;
  }

  if (dir.x != 0 || dir.y != 0) {
        // This is the "proper" way to get 0.707 for diagonals
        float length = (dir.x != 0 && dir.y != 0) ? 0.707f : 1.0f;
        Vector2 potentialPosition = {player->position.x, player->position.y};
        potentialPosition.x += dir.x * player->speed * length * dt;
        potentialPosition.y += dir.y * player->speed * length * dt;
        if(!Check_Collision(map,potentialPosition)){
          player->position.x = potentialPosition.x;
          player->position.y = potentialPosition.y;
        }
    }

}

void Draw_Player(Player* player){
  // Image i = LoadImage("char.png");
  // DrawTexture(*sprite,player->position.x, player->position.y, WHITE);
  Rectangle r= {16,0,27,64};
  DrawTextureRec(player->sprite,r,player->position, WHITE );
  // DrawRectangle(player->position.x, player->position.y,20,20, MAGENTA);
}

void Close_Player(Player* player){
  UnloadTexture(player->sprite);
}