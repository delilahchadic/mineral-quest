#include "character.h"

Character GetCharacterDefault(){
  Character character = {0};
  character.name = "henry";
  character.position = (Vector2){ 300,240 };
  character.dialogId = 1;
  Image image = LoadImage("p2.png");
  character.sprite = LoadTextureFromImage(image);
  UnloadImage(image);
  return character;
}

void Draw_Character(Character* character){
  // Image i = LoadImage("char.png");
  // DrawTexture(*sprite,player->position.x, player->position.y, WHITE);
  Rectangle r= {0,0,64,64};
  DrawTextureRec(character->sprite,r,character->position, WHITE );
  // DrawRectangle(player->position.x, player->position.y,20,20, MAGENTA);
}