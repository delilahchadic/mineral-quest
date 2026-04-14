#include "map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

void InitMap(Map* map){
  Init_Player(map);
  map->lastTileHeight = -1;
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
  map->camera.target = GetWorldToIso(map->player->position);
  map->camera.offset = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };// Center of the 800x450 screen
  map->camera.rotation = 0.0f;
  map->camera.zoom = 1.0f;
}

void Init_Player(Map* map){
  MapEntity* player = malloc(sizeof(MapEntity)); 
  player->type = ENTITY_PLAYER;
  player->state = NORMAL_STATE;
  player->position = (Vector2){30,30};
  Add_Entity(map,player);
  map->player = player;
  player->jumpoffset = 0.0f;
}

void Close_Map(Map* map){
  if(map == NULL){
    return;
  }
  MapEntity* tmp = NULL;
  tmp = map->entities;
  while(tmp != NULL){
    map->entities = tmp->next;
    free(tmp);
    tmp = map->entities;
  }
}

// This function takes your "Normal" coordinates and returns "Isometric" screen pixels
Vector2 GetWorldToIso(Vector2 worldPos) {
  Vector2 iso;
  // The Standard Formula:
  iso.x = (worldPos.x - worldPos.y);
  iso.y = (worldPos.x + worldPos.y) / 2.0f; // This /2 creates the 50% "squash"
  return iso;
}

void Draw_Map(Map* map) {
  BeginMode2D(map->camera);
    for (int y = 0; y < map->rows; y++) {
      for (int x = 0; x < map->columns; x++) {
        Draw_Tile(map,x,y);
      }
    }

    MapEntity* curr = map->entities;
      while(curr != NULL) {
        Draw_MapEntity(curr, map);
        curr = curr->next;
      }

    EndMode2D();
  
}

void Draw_Tile(Map* map, int x, int y){
  float h = map->grid[y][x].height * 8.0f;
      Color base = TILE_REGISTRY[map->grid[y][x].type].color;

      // Calculate 4 ground corners
      Vector2 g1 = GetWorldToIso((Vector2){ x * TILE_SIZE, y * TILE_SIZE });
      Vector2 g2 = GetWorldToIso((Vector2){ (x + 1) * TILE_SIZE, y * TILE_SIZE });
      Vector2 g3 = GetWorldToIso((Vector2){ (x + 1) * TILE_SIZE, (y + 1) * TILE_SIZE });
      Vector2 g4 = GetWorldToIso((Vector2){ x * TILE_SIZE, (y + 1) * TILE_SIZE });

      // Calculate 4 top corners (lifted by h)
      Vector2 t1 = { g1.x, g1.y - h };
      Vector2 t2 = { g2.x, g2.y - h };
      Vector2 t3 = { g3.x, g3.y - h };
      Vector2 t4 = { g4.x, g4.y - h };

      // Draw Walls (The 'sides' of the block)
      if (h > 0) {
        // Right Side (Darker)
        Color sideL = { (unsigned char)(base.r*0.8), (unsigned char)(base.g*0.8), (unsigned char)(base.b*0.8), 255 };
        Color sideR = { (unsigned char)(base.r*0.6), (unsigned char)(base.g*0.6), (unsigned char)(base.b*0.6), 255 };
        //  right side
        DrawTriangleFan((Vector2[]){ t3, g3, g2, t2}, 4, sideL);
        // back side
        DrawTriangleFan((Vector2[]){ t1, g1, g2, t2 }, 4, sideL);
        //front side
        DrawTriangleFan((Vector2[]){ t4, g4, g3, t3}, 4, sideR);
        
      }

      // Draw Top Face
      DrawTriangleFan((Vector2[]){ t1, t4, t3, t2 }, 4, base);
      // Grid Outlines
      DrawLineV(t1, t2, Fade(BLACK, 0.1f));
      DrawLineV(t2, t3, Fade(BLACK, 0.1f));
      DrawLineV(t3, t4, Fade(BLACK, 0.1f));
      DrawLineV(t4, t1, Fade(BLACK, 0.1f));
}

void Draw_MapEntity(MapEntity* entity,Map* map){
  Vector2 position = GetWorldToIso(entity->position);

  int tx = (int)(entity->position.x / TILE_SIZE);
  int ty = (int)(entity->position.y / TILE_SIZE);
  if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
      float hOffset = map->grid[ty][tx].height * 8.0f;
      position.y -= (hOffset + entity->jumpoffset); // Lift the character up!
  }

  Vector2 origin = { 16, 64 }; 
  Vector2 drawPos = { position.x - origin.x, position.y - origin.y };

  Rectangle r= {0,0,32,64};

  Texture2D* sprite = (entity->type == ENTITY_PLAYER) ? &PLAYER->sprite : GetSprite(entity->type, entity->id);
  DrawTextureRec(*sprite,r,drawPos, WHITE );
  // Draw a small gray ellipse at 'position' to ground the character
  DrawCircleGradient(position.x, position.y, 8, Fade(BLACK, 0.3f), BLANK);
  // //draws their name
  char* name = (entity->type == ENTITY_PLAYER) ? "player" : GetName(entity->type, entity->id);
  DrawText(name, drawPos.x, drawPos.y - 10, 10, COLOR_SUNKEN_INK);
  
}

void Update_Map(Map* map, bool moved){
  if(moved){
    Remove_Entity(map,map->player);
    Add_Entity(map, map->player);
  }

  float smoothness = 0.1f;
  Vector2 playerIsoPosition = GetWorldToIso(map->player->position);
  map->camera.target.x += (playerIsoPosition.x - map->camera.target.x) * smoothness;
  map->camera.target.y += (playerIsoPosition.y - map->camera.target.y) * smoothness;

}

void AdjustCamera(Map* map, bool dialog){
  if(dialog){
    map->camera.zoom += (1.2f - map->camera.zoom) * 0.05f;
  }else{
    map->camera.zoom += (1.0f - map->camera.zoom) * 0.05f;
  }
}

void Remove_Entity(Map* map, MapEntity* entity){
  if(map==NULL || entity == NULL) return;

  if(map->entities == entity){
    map->entities = entity->next;
    entity->next =  NULL;
    return;
  }

  MapEntity* curr = map->entities;
  while(curr->next != NULL && curr->next != entity){
    curr = curr->next;
  }
  if(curr->next == entity){
    curr->next = entity->next;
    entity->next = NULL;
    return;
  }
}

void Add_Entity(Map* map, MapEntity* entity){
  if(map->entities == NULL || (entity->position.x + entity->position.y) < map->entities->position.x + map->entities->position.y){
    entity->next = map->entities;
    map->entities = entity;
    return;
  }

  MapEntity* curr = map->entities;
  while(curr->next != NULL && (curr->next->position.x + curr->next->position.y) < (entity->position.x + entity->position.y)){
    curr = curr->next;
  }
  entity->next= curr->next; 
  curr->next = entity;
  return;
}

int PollDialog(Map* map){
  MapEntity* tmp = map->entities;
  while (tmp != NULL) {
    if(tmp->trait_flags & TALK){
      if(Vector2Distance(map->player->position, tmp->position) <50.f){
        return GetDialogID(tmp->type, tmp->id);
      }
    }
    tmp = tmp->next;
  }
  return -1;
}

