#include "map.h"

TileDefinition TILE_REGISTRY[6] = {
    { TILE_WATER, true,  0.5f, 101, COLOR_CERULEAN_DUSTY},
    { TILE_STONE, false,  1.0f, 102, COLOR_TEXAS_HAZE    },
    { TILE_SAND,  false, 0.4f, 103, COLOR_DUSTY_ROSE    },
    { TILE_DIRT,  false, 0.8f, 104, COLOR_DUSTY_SALMON  },
    { TILE_GRASS, false, 0.9f, 105, COLOR_DUSTY_SAP          },
    { TILE_ROAD,  false, 1.0f, 106, COLOR_ASPHALT       }
};

void LoadMapEntityFile(const char* filename, Map* map){
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

void InitMap(Map* map){
  map->entities = NULL;
  LoadMapGridFile("data/maps/highway.map", map);
  LoadMapEntityFile("data/maps/highway.ents", map);
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
}

void CloseMap(Map* map){
  if(map == NULL){
    return;
  }
}


void Draw_Map(Map* map) {
    for (int y = 0; y < map->rows; y++) {
        for (int x = 0; x < map->columns; x++) {
            Color tileColor = TILE_REGISTRY[map->grid[y][x]].color;
            // Tile t = world->types[world->tiles[y][x]];
            DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
            // Draw a subtle grid line
            DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Fade(COLOR_SUNKEN_INK, 0.1f));
        }
    }

    MapEntity* tmp = map->entities;
    while(tmp!=NULL){
      Color color;
      switch (tmp->type)
      {
      case ENTITY_CHARACTER:
        color = COLOR_DUSTY_ROSE;
        break;
      case ENTITY_PLANT:
        color = COLOR_CERULEAN_TEAL;
        break;
      case ENTITY_DECOR:
        color = COLOR_TAROT_GOLD;
        break;
      case ENTITY_ITEM:
        color = COLOR_DUSTY_CORAL;
        break;
      default:
        break;
      }
      DrawRectangle(tmp->position.x + 2, tmp->position.y + 14, 16, 4, Fade(BLACK, 0.3f));
    
      // Draw the actual entity
      if(tmp->type==0){
        Character* c = tmp->data.character;
        Rectangle r= {16,0,27,64};
        DrawTextureRec(c->sprite,r,tmp->position, WHITE );
        DrawText(tmp->data.character->name, tmp->position.x, tmp->position.y - 10, 10, COLOR_SUNKEN_INK);
      }else{
        DrawRectangle(tmp->position.x, tmp->position.y, 16, 16, color);
        DrawText(tmp->name, tmp->position.x, tmp->position.y - 10, 10, COLOR_SUNKEN_INK);
      }
      
      
      // Draw the name if you want to see who's who
      
      tmp = tmp->next;
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
      if (curr->type == ENTITY_PLANT || curr->type == ENTITY_DECOR) {
        
        // Define the player's small foot-patch rectangle
        Rectangle playerFeet = { footLeft, footTop, 19, 9 }; 
        
        // Define the entity's rectangle (matching your 16x16 Draw size)
        Rectangle entityBox = { curr->position.x, curr->position.y, 16, 16 };
        
        if (CheckCollisionRecs(playerFeet, entityBox)) {
            return true; // Stop the player!
        }
    }
        // If we add an 'is_blocking' flag to entities later, check it here
        curr = curr->next;
    }

    return false;
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

void Update_Map(Player* player,Map* map, Dialog_Manager* manager){
   MapEntity* tmp = map->entities;
    while (tmp != NULL) {
      if(tmp->type==0){
        Character* c = tmp->data.character;
        if(Vector2Distance(player->position, tmp->position) <50.f){
          if (IsKeyPressed(KEY_E) && !manager->active) {
              // Tell the manager to look up the ID stored on the character

              Set_Active_Dialog(manager, c->dialogId);
              manager->active = true;
          }
        }
      }
    
        tmp = tmp->next;
    }
}

void Add_Entity(Map* map, MapEntity* entity){
  if(map->entities == NULL){
    map->entities = entity;
    return;
  }
  MapEntity* tmp = map->entities;
  while(tmp->next != NULL){
    tmp = tmp->next;
  }
  tmp->next = entity;
  return;

}