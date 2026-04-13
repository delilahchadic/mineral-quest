#include "map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60

// registry types temporary till we move into a dedicated register
TileDefinition TILE_REGISTRY[6] = {
  { TILE_WATER, true,  0.5f, 101, COLOR_CERULEAN_DUSTY},
  { TILE_GRASS, false,  1.0f, 102, COLOR_DUSTY_SAP    },
  { TILE_SAND,  false, 0.4f, 103, COLOR_CERULEAN_BERYL    },
  { TILE_DIRT,  false, 0.8f, 104, COLOR_DUSTY_SALMON  },
  { TILE_STONE, false, 0.9f, 105, COLOR_DUSTY_SAP          },
  { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT       }
};

void Init_Player(Map* map){
  MapEntity* player = malloc(sizeof(MapEntity)); 
  player->type = ENTITY_PLAYER;
  player->state = NORMAL_STATE;
  player->position = (Vector2){30,30};
  player->sprite = &PLAYER->sprite;
  strncpy(player->name, "player", sizeof(player->name) - 1);
  player->name[sizeof(player->name) - 1] = '\0'; 
  Add_Entity(map,player);
  map->player = player;
  player->jumpoffset = 0.0f;
}

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

        MapEntity* curr = map->entities;
      while(curr != NULL) {
        int tx = (int)(curr->position.x / TILE_SIZE);
        int ty = (int)(curr->position.y / TILE_SIZE);
        
        if (tx == x && ty == y) {
          Draw_MapEntity(curr, map);
        }
        curr = curr->next;
      }
    }}

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
  DrawTextureRec(*entity->sprite,r,drawPos, WHITE );
  // Draw a small gray ellipse at 'position' to ground the character
  DrawCircleGradient(position.x, position.y, 8, Fade(BLACK, 0.3f), BLANK);
  //draws their name
  if(entity->type== ENTITY_CHARACTER){
    Character* c = entity->data.character;
    DrawText(c->name, drawPos.x, drawPos.y - 10, 10, COLOR_SUNKEN_INK);
  }else if(entity->type == ENTITY_PLANT){
    Plant* c = entity->data.plant;
    DrawText(c->species_name,drawPos.x,drawPos.y - 10, 10, COLOR_SUNKEN_INK);
  } else{
      DrawText(entity->name, drawPos.x, drawPos.y - 10, 10, COLOR_SUNKEN_INK);
  }
  
}

void Handle_Input(Map* map){
  float dt = GetFrameTime();
  Vector2 dir = { 0, 0 };

  if (IsKeyDown(KEY_W)) dir.y -= 1;
  if (IsKeyDown(KEY_A)) dir.x -= 1;
  if (IsKeyDown(KEY_D)) dir.x += 1;
  if (IsKeyDown(KEY_S)) dir.y += 1;
  if (IsKeyDown(KEY_SPACE)){
    if(map->player->state != JUMPING_STATE){
      map->player->state = JUMPING_STATE;
      map->player->verticalVelocity= 8.0f;
    }
  }
  if (dir.x != 0 || dir.y != 0) {
    // This is the "proper" way to get 0.707 for diagonals
    float length = (dir.x != 0 && dir.y != 0) ? 0.707f : 1.0f;
    float moveSpeed = PLAYER->speed * length * dt;
      // 1. Try X movement
      // Vector2 nextX = map->player->position;
    if(map->player->velocity.x < PLAYER->maxSpeed)
      map->player->velocity.x += dir.x * moveSpeed;
    
    if(map->player->velocity.y < PLAYER->maxSpeed)
      map->player->velocity.y += dir.y * moveSpeed;
    
  }

}

bool Check_Collision(Map* map, Vector2 nextPos) {
    // 1. Get the tile coordinates for the CURRENT position
    int curX = (int)(map->player->position.x / TILE_SIZE);
    int curY = (int)(map->player->position.y / TILE_SIZE);
    
    // Safety check
    if (curX < 0 || curX >= map->columns || curY < 0 || curY >= map->rows) return true;

    // IMPORTANT: Calculate current world height (Tile + Jump)
    float currentWorldHeight = (float)map->grid[curY][curX].height + (map->player->jumpoffset / 8.0f);

    // Collision Box
    float footLeft   = nextPos.x - 14;
    float footRight  = nextPos.x + 14;
    float footTop    = nextPos.y - 8;
    float footBottom = nextPos.y + 8;

    if (footLeft < 0 || footRight >= map->pixel_width || footTop < 0 || footBottom >= map->pixel_height) return true;

    int checkX[] = { (int)(footLeft / TILE_SIZE), (int)(footRight / TILE_SIZE) };
    int checkY[] = { (int)(footTop / TILE_SIZE), (int)(footBottom / TILE_SIZE) };

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float targetHeight = (float)map->grid[checkY[j]][checkX[i]].height;
            bool isBlocking = TILE_REGISTRY[map->grid[checkY[j]][checkX[i]].type].is_blocking;

            // Use 1.1f to allow for tiny float errors when stepping up 1 unit
            if (isBlocking || targetHeight > (currentWorldHeight + 1.1f)) {
                return true; 
            }
        }
    }
    return false;
}

void Apply_Gravity(Map* map) {
  int tx = (int)(map->player->position.x / TILE_SIZE);
  int ty = (int)(map->player->position.y / TILE_SIZE);
  if (tx < 0 || tx >= map->columns || ty < 0 || ty >= map->rows) return;

  // If we move to a lower tile, the "jumpoffset" needs to increase 
  // to keep us at the same visual height while we start falling.
  int currentTileHeight = map->grid[ty][tx].height;

  if (map->lastTileHeight != -1 && currentTileHeight < map->lastTileHeight) {
      // We just stepped down! Add the difference to jumpoffset so we don't "snap" down.
      map->player->jumpoffset += (map->lastTileHeight - currentTileHeight) * 8.0f;
      map->player->state = JUMPING_STATE;
  }
  map->lastTileHeight = currentTileHeight;

  // Standard Gravity Logic
  if (map->player->state == JUMPING_STATE || map->player->jumpoffset > 0.0f) {
      map->player->verticalVelocity -= 1.0f; 
      map->player->jumpoffset += map->player->verticalVelocity;
  }

  // Hit the floor
  if (map->player->jumpoffset <= 0.0f) {
      map->player->jumpoffset = 0.0f;
      map->player->verticalVelocity = 0.0f;
      map->player->state = NORMAL_STATE;
  }
}

void Update_Physics(Map* map){
  Apply_Gravity(map);
  Apply_Friction(map);
  Resolve_Movement(map);
}

void Resolve_Movement(Map* map){
  Vector2 nextX = map->player->position;
  bool moved = false;
  nextX.x += map->player->velocity.x;
  if (!Check_Collision(map, nextX)) {
    map->player->position.x = nextX.x;
    moved = true;
  }

  Vector2 nextY = map->player->position;
  nextY.y += map->player->velocity.y;
  if (!Check_Collision(map, nextY)) {
    map->player->position.y = nextY.y;
    moved = true;
  }

  if(moved){
    Remove_Entity(map, map->player);
    Add_Entity(map, map->player);
  }
}

void Apply_Friction(Map* map){
   if(map->player->velocity.x != 0 || map->player->velocity.y != 0 ){
    map->player->velocity.x *= 0.9f;
    map->player->velocity.y *= 0.9f;
  }
}

void Update_Map(Map* map, DialogManager* manager){
   MapEntity* tmp = map->entities;
    while (tmp != NULL) {
      if(tmp->type== ENTITY_CHARACTER){
        Character* c = tmp->data.character;
        if(Vector2Distance(map->player->position, tmp->position) <50.f){
          if (IsKeyPressed(KEY_E) && !manager->active) {
              // Tell the manager to look up the ID stored on the character
              Set_Active_Dialog(manager, c->dialogId);
              manager->active = true;
          }
        }
      }
    
        tmp = tmp->next;
    }
    // readjust camera to where the player is
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

