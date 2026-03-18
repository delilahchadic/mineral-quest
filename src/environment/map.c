#include "map.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TARGET_FPS 60
// registry types temporary till we move into a dedicated register
TileDefinition TILE_REGISTRY[6] = {
    { TILE_WATER, true,  0.5f, 101, COLOR_CERULEAN_DUSTY},
    { TILE_STONE, false,  1.0f, 102, COLOR_TEXAS_HAZE    },
    { TILE_SAND,  false, 0.4f, 103, COLOR_DUSTY_ROSE    },
    { TILE_DIRT,  false, 0.8f, 104, COLOR_DUSTY_SALMON  },
    { TILE_GRASS, false, 0.9f, 105, COLOR_DUSTY_SAP          },
    { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT       }
};

void LoadMapEntityFile(const char* filename, Map* map){
  map->entities = NULL;
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  fgets(line, sizeof(line),file);//header
  // int col =0;
  while (fgets(line, sizeof(line), file)) {
    // col = 0;
    line[strcspn(line, "\n")] = 0;
    char* typeToken = strtok(line,",");
    char* xToken = strtok(NULL,",");
    char* yToken = strtok(NULL,",");
    char* nameToken = strtok(NULL,",");
    if(typeToken && xToken && yToken){
      MapEntity* m = malloc(sizeof(MapEntity)); 
      if (m == NULL) continue; // Safety check
      m->type = (EntityType) atoi(typeToken);
      m->position = (Vector2) {(float)atoi(xToken), (float)atoi(yToken)};
      m->next = NULL;

      if(m->type == ENTITY_CHARACTER){
        m->data.character = &CHARACTER_REGISTRY[atoi(nameToken)];
        m->sprite = &m->data.character->sprite;
      }else if(m->type == ENTITY_PLANT){
        m->data.plant = &PLANT_REGISTRY[atoi(nameToken)];
        m->sprite = &m->data.plant->sprite;
      }else{
        strncpy(m->name, nameToken, sizeof(m->name) - 1);
        m->name[sizeof(m->name) - 1] = '\0';
      }      
      Add_Entity(map,m);
    }
    
  }
    fclose(file);
}

void LoadMapGridFile(const char* filename, Map* map){
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "Failed to open %s", filename);
    return;
  }
  TraceLog(LOG_INFO,"File Loaded - %s", filename);
  char line[1024];
  fgets(line, sizeof(line),file);
  char* rowToken = strtok(line,",");
  char* colToken = strtok(NULL,",");
  char* nameToken = strtok(NULL,",");
  
  if(rowToken && colToken){
    map->rows = atoi(rowToken);
    map->columns = atoi(colToken);
    strncpy(map->name, nameToken, sizeof(map->name) - 1);
    map->name[sizeof(map->name) - 1] = '\0'; 
  }
  int row =0;
  int col =0;
  while (fgets(line, sizeof(line), file) && row<map->rows) {
    // col = 0;
    line[strcspn(line, "\n")] = 0;
    char* colToken = strtok(line,",");
    // map->grid[row][0] = atoi(colToken);
    for (col = 0;col<map->columns;col++){
      map->grid[row][col] = atoi(colToken);
      colToken = strtok(NULL,",");
      
    }
    
    row++;
  }
    fclose(file);
}

void Init_Player(Map* map){
  MapEntity* player = malloc(sizeof(MapEntity)); 
  player->type = ENTITY_PLAYER;
  player->position = (Vector2){0,0};
  player->sprite = &PLAYER->sprite;
  strncpy(player->name, "player", sizeof(player->name) - 1);
  player->name[sizeof(player->name) - 1] = '\0'; 
  Add_Entity(map,player);
  map->player = player;
}

void InitMap(Map* map){
  LoadMapGridFile("data/maps/highway.map", map);
  LoadMapEntityFile("data/maps/highway.ents", map);
  Init_Player(map);
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
  map->camera.target = map->player->position;
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


void Draw_Map(Map* map) {
  BeginMode2D(map->camera);
    Draw_Tiles(map);
    MapEntity* curr = map->entities;
    while(curr!=NULL){
      // Draw the actual entity
      Draw_MapEntity(curr);
      curr = curr->next;
    }
    EndMode2D();
  
}

void Draw_Tiles(Map* map){
  //draws the world
  for (int y = 0; y < map->rows; y++) {
    for (int x = 0; x < map->columns; x++) {
      Color tileColor = TILE_REGISTRY[map->grid[y][x]].color;
      DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
      // Draw a subtle grid line
      DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Fade(COLOR_SUNKEN_INK, 0.1f));
    }
  }
}

void Draw_MapEntity(MapEntity* entity){
  if(entity->type== ENTITY_CHARACTER){
        Character* c = entity->data.character;
        Rectangle r= {0,0,32,64};
        DrawTextureRec(*entity->sprite,r,entity->position, WHITE );
        DrawText(c->name, entity->position.x, entity->position.y - 10, 10, COLOR_SUNKEN_INK);
      }else if(entity->type == ENTITY_PLANT){
        Plant* c = entity->data.plant;
        Rectangle r= {0,0,32,64};
        DrawTextureRec(*entity->sprite,r,entity->position, WHITE );
        DrawText(c->species_name, entity->position.x, entity->position.y - 10, 10, COLOR_SUNKEN_INK);
      }else if(entity->type == ENTITY_PLAYER){
        Rectangle r= {0,0,32,64};
        DrawTextureRec(*entity->sprite,r,entity->position, WHITE );
      } else{
        DrawRectangle(entity->position.x, entity->position.y, 16, 16, COLOR_PULP_PAPER);
        DrawText(entity->name, entity->position.x, entity->position.y - 10, 10, COLOR_SUNKEN_INK);
      }
}

bool Check_Collision(Map* map, Vector2 nextPos) {
    // We define a small "collision patch" at the feet.
    // 27px wide sprite, so we check from x+4 to x+23 (narrower for forgiveness)
    // 64px tall sprite, so we check near the bottom (y+60)
    
    float footLeft   = nextPos.x + 4;
    float footRight  = nextPos.x + 23;
    float footBottom = nextPos.y + 63; // 63 is the last pixel of the 64px height
    float footTop    = nextPos.y + 54; // A 10-pixel high collision strip

    // 1. Map Boundary Check (Keep him inside the world)
    if (footLeft < 0 || footRight >= map->pixel_width || 
        footTop < 0 || footBottom >= map->pixel_height) {
        return true; 
    }

    // 2. Multi-Point Tile Check
    // We check the left-foot corner and the right-foot corner.
    int checkX[] = { (int)(footLeft / TILE_SIZE), (int)(footRight / TILE_SIZE) };
    int checkY[] = { (int)(footTop / TILE_SIZE), (int)(footBottom / TILE_SIZE) };

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int tileID = map->grid[checkY[j]][checkX[i]];
            if (TILE_REGISTRY[tileID].is_blocking) {
                return true;
            }
        }
    }

    // 3. Entity Check (Future proofing for those Barrels/Plants)
    MapEntity* curr = map->entities;
    while (curr != NULL) {
      if(curr->type != ENTITY_PLAYER){
        if (curr->type == ENTITY_PLANT) {
          Plant* plant = curr->data.plant;

          float plantLeft   = curr->position.x ;
          float plantTop    = curr->position.y + 54; 

          // // Define the player's small foot-patch rectangle
          Rectangle playerFeet = { footLeft, footTop, 19, 9 }; 
          // // Define the entity's rectangle (matching your 16x16 Draw size)
          Rectangle entityBox = { plantLeft, plantTop, plant->hitboxwidth, plant->hitboxheight};
          
          if (CheckCollisionRecs(playerFeet, entityBox)) {
              return true; // Stop the player!
          }
        }
      }
      
        // If we add an 'is_blocking' flag to entities later, check it here
        curr = curr->next;
    }

    return false;
}

void Handle_Input(Map* map){
  float dt = GetFrameTime();
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
        Vector2 potentialPosition = {map->player->position.x, map->player->position.y};
        potentialPosition.x += dir.x * PLAYER->speed * length * dt;
        potentialPosition.y += dir.y * PLAYER->speed * length * dt;
        if(!Check_Collision(map,potentialPosition)){
          bool ychanged=map->player->position.y != potentialPosition.y;
          
          map->player->position.x = potentialPosition.x;
          map->player->position.y = potentialPosition.y;
          if(ychanged){
            Remove_Entity(map, map->player);
            Add_Entity(map, map->player);
          }
        }
    }

}

void Update_Map(Map* map, Dialog_Manager* manager){
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
  map->camera.target.x += (map->player->position.x - map->camera.target.x) * smoothness;
  map->camera.target.y += (map->player->position.y - map->camera.target.y) * smoothness;

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
  if(map->entities == NULL || entity->position.y < map->entities->position.y){
    entity->next = map->entities;
    map->entities = entity;
    return;
  }

  MapEntity* curr = map->entities;
  while(curr->next != NULL && curr->next->position.y < entity->position.y){
    curr = curr->next;
  }
  entity->next= curr->next; 
  curr->next = entity;
  return;
}