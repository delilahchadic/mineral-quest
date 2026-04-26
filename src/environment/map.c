#include "map.h"

void InitMap(Map* map){
  Init_Player(map);
  map->lastTileHeight = -1;
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
}

void InitNewMap(Map* map,char* name,int columns, int rows){
  memset(map, 0, sizeof(Map));
  snprintf(map->name, sizeof(map->name),"%s", name);
  map->name[sizeof(map->name) - 1] = '\0';
  map->rows = rows;
  map->columns = columns;
  for(int i =0;i<map->rows;i++){
    for(int j =0;j<map->columns;j++){
      map->grid[i][j].height = 0;
      map->grid[i][j].type = TILE_GRASS;
    }
  }

  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
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

// This function takes your "Normal" coordinates and returns "Isometric" screen pixels
Vector2 GetIsoWorldToGrid(Vector2 worldPos) {
    Vector2 grid;
    float halfW = TILE_SIZE / 1.0f;
    float halfH = TILE_SIZE / 2.0f;

    // The "Inverse" Isometric Formula:
    // This turns the 'Diamond' pixels back into 'Square' indices
    grid.x = (worldPos.x / halfW + worldPos.y / halfH) / 2.0f;
    grid.y = (worldPos.y / halfH - worldPos.x / halfW) / 2.0f;

    return grid;
}

Vector2 GetGridToIsoWorld(int x, int y) {
    float worldX = (x - y) * (TILE_SIZE / 2.0f);
    float worldY = (x + y) * (TILE_SIZE / 2.0f);

    return (Vector2){ worldX, worldY };
}

Vector2 GetIsoWorldToGridWithHeight(Map* map, Vector2 screenWorldPos) {
    // 1. Define your max height (matches your grid limits)
    // If your max height is 10, start there.
    const int MAX_HEIGHT = 200;
    const float HEIGHT_STEP = 8.0f; // From your Draw_Tile: height * 8.0f

    for (int h = MAX_HEIGHT; h >= 0; h--) {
        // 2. Offset the Y coordinate to 'drop' the screen click
        // to the level of the current height slice.
        float pixelOffset = h * HEIGHT_STEP;
        Vector2 testPos = { screenWorldPos.x, screenWorldPos.y + pixelOffset };

        // 3. Use your existing math to find what grid cell that corresponds to
        Vector2 grid = GetIsoWorldToGrid(testPos);

        int ix = (int)grid.x;
        int iy = (int)grid.y;

        // 4. Validate the grid index
        if (ix >= 0 && ix < map->columns && iy >= 0 && iy < map->rows) {
            // 5. Check: Is the tile at this grid coordinate actually at this height?
            // We check >= because you might be clicking the "side" of a tall block.
            if (map->grid[iy][ix].height >= h) {
                return (Vector2){ (float)ix, (float)iy };
            }
        }
    }

    // Fallback: If nothing was hit, return the flat ground (height 0)
    return GetIsoWorldToGrid(screenWorldPos);
}

void Draw_Map(Map* map, Camera2D* camera) {
  BeginMode2D(*camera);
    Vector2 tl_corner = GetScreenToWorld2D((Vector2){0,0}, *camera);
    Vector2 tr_corner = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,0}, *camera);
    Vector2 bl_corner = GetScreenToWorld2D((Vector2){0,SCREEN_HEIGHT}, *camera);
    Vector2 br_corner = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,SCREEN_HEIGHT}, *camera);

    Vector2 g1 = GetIsoWorldToGrid(tl_corner);
    Vector2 g2 = GetIsoWorldToGrid(tr_corner);
    Vector2 g3 = GetIsoWorldToGrid(bl_corner);
    Vector2 g4 = GetIsoWorldToGrid(br_corner);
    float min_x = fminf(fminf(g1.x, g2.x), fminf(g3.x, g4.x)) -2;
    float max_x = fmaxf(fmaxf(g1.x, g2.x), fmaxf(g3.x, g4.x))+2;

    float min_y = fminf(fminf(g1.y, g2.y), fminf(g3.y, g4.y))-2;
    float max_y = fmaxf(fmaxf(g1.y, g2.y), fmaxf(g3.y, g4.y))+2;

    min_x = min_x < 0 ? 0: min_x;
    min_y = min_y < 0 ? 0: min_y;
    max_x = max_x > map->columns ? map->columns: max_x;
    max_y = max_y > map->rows ? map->rows: max_y;

    for (int y = min_y; y < max_y; y++) {
      for (int x = min_x; x < max_x; x++) {
        Draw_Tile(map,x,y);
      }
    }

    MapEntity* curr = map->entities;
    while(curr != NULL) {
      float gx = curr->position.x / TILE_SIZE;
    float gy = curr->position.y / TILE_SIZE;

    // 2. Compare against your calculated min/max bounds
    // We use a small buffer (+1/-1) so sprites don't pop out at the very edge
    if (gx >= min_x - 1 && gx <= max_x + 1 &&
        gy >= min_y - 1 && gy <= max_y + 1) {
        Draw_MapEntity(curr, map);
    }
    curr = curr->next;
    }

    EndMode2D();

}

void Draw_Tile(Map* map, int x, int y){
  float h = map->grid[y][x].height * 8.0f;
  Color base = TILE_REGISTRY[map->grid[y][x].type].color;
  bool blocking = TILE_REGISTRY[map->grid[y][x].type].is_blocking;
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
  if (h > 0 && !blocking){
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

  if (map->grid[y][x].type == TILE_WATER) {
    DrawWaterTile(t1,t2,t3,t4,x,y);
  }else{
    DrawTriangleFan((Vector2[]){ t1, t4, t3, t2 }, 4, base);
    DrawLineV(t1, t2, Fade(BLACK, 0.1f));
    DrawLineV(t2, t3, Fade(BLACK, 0.1f));
    DrawLineV(t3, t4, Fade(BLACK, 0.1f));
    DrawLineV(t4, t1, Fade(BLACK, 0.1f));
  }

}

void  Draw_MapEntity(MapEntity* entity,Map* map){
  Vector2 position = GetWorldToIso(entity->position);

  int tx = (int)(entity->position.x / TILE_SIZE);
  int ty = (int)(entity->position.y / TILE_SIZE);
  if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
      float hOffset = map->grid[ty][tx].height * 8.0f;
      position.y -= (hOffset + entity->jumpoffset); // Lift the character up!
  }
  Texture2D* sprite = GetSprite(entity->type, entity->id);
  // Calculate vertical offset based on the actual (potentially scaled) height
    float renderHeight = (entity->type == ENTITY_ITEM) ? (sprite->height * 0.5f) : (float)sprite->height;
    float renderWidth = (entity->type == ENTITY_ITEM) ? (sprite->width * 0.5f) : (float)sprite->width;

    // Center the sprite horizontally (width/2) and place bottom at isoPos.y
    Vector2 drawPos = { position.x - (renderWidth / 2), position.y - renderHeight };

  if(entity->type == ENTITY_ITEM ){
    DrawTextureEx(*sprite,drawPos,0.0,0.5, WHITE );
  }else{
    // Rectangle r= (Rectangle){0,0,32,64};
    DrawTextureV(*sprite,drawPos, WHITE );
    // Draw a small gray ellipse at 'position' to ground the character

    DrawCircleGradient(position.x, position.y, 8, Fade(BLACK, 0.3f), BLANK);
    // //draws their name
    char* name = (entity->type == ENTITY_PLAYER) ? "player" : GetName(entity->type, entity->id);
    DrawText(name, drawPos.x, drawPos.y - 10, 10, COLOR_SUNKEN_INK);
  }

}

void DrawWaterTile(Vector2 t1, Vector2 t2, Vector2 t3, Vector2 t4, int x, int y) {
    // 1. Base Water (unchanged, but noted: Indanthrone Blue looks great here)
    float pulse = sinf(GetTime()) * 30.0f;
    Color waterColor = COLOR_BEAVIS_SHIRT;
    waterColor.a = 150 + (unsigned char)pulse;
    DrawTriangleFan((Vector2[]){ t1, t4, t3, t2 }, 4, waterColor);

    // 2. The Sparkle Logic
    float tileSeed = (float)(x * 12.9898f + y * 78.233f);
    float sparkleTime = sinf(GetTime() * 2.5f + tileSeed); // Slightly faster pulse

    if (sparkleTime > 0.97f) {
        // Use fmodf for smoother, overflow-safe randomness
        float offsetX = fmodf(tileSeed * 43758.5453f, (float)TILE_SIZE);
        float offsetY = fmodf(tileSeed * 12345.6789f, (float)TILE_SIZE / 2.0f);

        Vector2 sparklePos = { t1.x + offsetX - (int)(TILE_SIZE/2), t1.y + offsetY };
        float sizePulse = (sinf(GetTime() * 8.0f + tileSeed) + 1.0f) * 1.5f + 1.0f;

        BeginBlendMode(BLEND_ADDITIVE);
            // Using a slightly warmer glow color (like COLOR_CELADON)
            // makes the Indanthrone core pop even harder.
            DrawSimpleSparkle(sparklePos, COLOR_INDANTHRONE_BLUE, sizePulse);
            DrawCircleV(sparklePos, sizePulse * 0.3f, WHITE);
        EndBlendMode();
    }
}

void DrawSimpleSparkle(Vector2 pos, Color color, float size) {
    // Bloom & Glow
    DrawCircleV(pos, size * 2.5f, Fade(color, 0.1f));
    DrawCircleV(pos, size, Fade(color, 0.4f));

    float thickness = size * 0.25f;
    // Vertical line (full size)
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size}, thickness, color);
    // Horizontal line (slightly shorter for that "anamorphic" lens look)
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y}, (Vector2){pos.x + (size * 0.8f), pos.y}, thickness, color);

    // The "Hot" Center
    DrawCircleV(pos, size * 0.4f, WHITE);
}

void Update_Map(Map* map, bool moved){
  if(moved){
    Remove_Entity(map,map->player);
    Add_Entity(map, map->player);
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
    if(entity->type != ENTITY_PLAYER) free(entity);
    return;
  }
}

void Add_Entity(Map* map, MapEntity* entity){

  if(map->entities == NULL ||
    (entity->position.x + entity->position.y) <
    map->entities->position.x + map->entities->position.y){
    entity->next = map->entities;
    map->entities = entity;
    return;
  }

  MapEntity* curr = map->entities;
  while(curr->next != NULL &&
    (curr->next->position.x + curr->next->position.y  <
  (entity->position.x + entity->position.y))) {
    curr = curr->next;
  }

  entity->next= curr->next;
  curr->next = entity;
  return;
}

MapEntity* PollTrait(Map* map, TraitFlags trait, float distance){
  MapEntity* tmp = map->entities;
  while (tmp != NULL) {
    if(tmp->trait_flags & trait){
      if(Vector2Distance(map->player->position, tmp->position) < distance){
        return tmp;
      }
    }
    tmp = tmp->next;
  }
  return NULL;
}
