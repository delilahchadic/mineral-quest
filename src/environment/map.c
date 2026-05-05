#include "map.h"
#include "defs/types_entities.h"
#include "defs/types_systems.h"
#include "raylib.h"
#include "registry/mineral_register.h"

typedef enum ElementType{
    ELEMENT_NONE,
    ELEMENT_FIRE,
    ELEMENT_WATER
}ElementType;
void InitMap(Map* map){
  Init_Player(map);
  map->lastTileHeight = -1;
  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
  map->is_ready = true;
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
      map->grid[i][j].isoPos = GetWorldToIso((Vector2){ j * TILE_SIZE, i * TILE_SIZE });
    }
  }

  map->pixel_width = map->columns * TILE_SIZE;
  map->pixel_height = map->rows * TILE_SIZE;
  map->is_ready = true;
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
    const int MAX_HEIGHT = 150;
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
    if (map == NULL || !map->is_ready) return;

    BeginMode2D(*camera);

    // 1. Get Screen Corners
    Vector2 tl = GetScreenToWorld2D((Vector2){0,0}, *camera);
    Vector2 tr = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,0}, *camera);
    Vector2 bl = GetScreenToWorld2D((Vector2){0,SCREEN_HEIGHT}, *camera);
    Vector2 br = GetScreenToWorld2D((Vector2){SCREEN_WIDTH,SCREEN_HEIGHT}, *camera);

    // 2. Convert corners to Grid indices
    Vector2 g1 = GetIsoWorldToGrid(tl);
    Vector2 g2 = GetIsoWorldToGrid(tr);
    Vector2 g3 = GetIsoWorldToGrid(bl);
    Vector2 g4 = GetIsoWorldToGrid(br);

    // 3. THE "BRUTE FORCE" PADDING
    // We expand the bounds by 25 tiles in every direction.
    // This ensures that even very tall tiles from "off-screen" are drawn.
    int min_x = (int)fminf(fminf(g1.x, g2.x), fminf(g3.x, g4.x)) - 25;
    int max_x = (int)fmaxf(fmaxf(g1.x, g2.x), fmaxf(g3.x, g4.x)) + 25;
    int min_y = (int)fminf(fminf(g1.y, g2.y), fminf(g3.y, g4.y)) - 25;
    int max_y = (int)fmaxf(fmaxf(g1.y, g2.y), fmaxf(g3.y, g4.y)) + 25; // Huge bottom buffer

    // 4. HARD CLAMPING TO ACTUAL MAP LIMITS
    // This is the only way to ensure we don't crash but also don't stop at 48/50.
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x >= map->columns) max_x = map->columns - 1;
    if (max_y >= map->rows)    max_y = map->rows - 1;

    // 5. Safe Bucket Allocation
    // MapEntity** buckets = (MapEntity**)calloc(map->rows, sizeof(MapEntity*));
    memset(map->buckets, 0, sizeof(map->buckets));
    MapEntity* e = map->entities;
    while (e != NULL) {
        int ty = (int)(e->position.y / TILE_SIZE);
        if (ty >= 0 && ty < map->rows) {
            e->next_in_bucket = map->buckets[ty];
            map->buckets[ty] = e;
        }
        e = e->next;
    }

        // 6. THE LOOP: Use <= to ensure the last index (49) is hit.

for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            Draw_Tile(map, x, y);
            MapEntity* curr = map->buckets[y];
            while (curr != NULL) {
                int tx = (int)(curr->position.x / TILE_SIZE);
                if (tx == x) {
                    Draw_MapEntity(curr, map);
                }
                curr = curr->next_in_bucket;
            }
        }
    }

    BeginBlendMode(BLEND_ADDITIVE);
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if(map->grid[y][x].type ==  TILE_WATER){
                DrawWaterEffects(map,x,y);
            }
        }
    }

    EndBlendMode();
    EndMode2D();


    DrawFPS(10, 10);
}

void Draw_Tile(Map* map, int x, int y) {
    // BOUNDS CHECK: Ensure we aren't accessing garbage memory
    if (x < 0 || x >= map->columns || y < 0 || y >= map->rows) return;

    float h = map->grid[y][x].height * 8.0f;
    Color base = TILE_REGISTRY[map->grid[y][x].type].color;
    bool blocking = TILE_REGISTRY[map->grid[y][x].type].is_blocking;

    Vector2 g1 = map->grid[y][x].isoPos;
    Vector2 g2 = { g1.x + TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) };
    Vector2 g4 = { g1.x - TILE_SIZE, g1.y + (TILE_SIZE / 2.0f) };
    Vector2 g3 = { g1.x,             g1.y + TILE_SIZE };

    Vector2 t1 = { g1.x, g1.y - h };
    Vector2 t2 = { g2.x, g2.y - h };
    Vector2 t3 = { g3.x, g3.y - h };
    Vector2 t4 = { g4.x, g4.y - h };

    // WALL DRAWING with strict neighbor checking
    if (h > 0 && !blocking) {
        Color sideL = { (unsigned char)(base.r*0.8), (unsigned char)(base.g*0.8), (unsigned char)(base.b*0.8), 255 };
        Color sideR = { (unsigned char)(base.r*0.6), (unsigned char)(base.g*0.6), (unsigned char)(base.b*0.6), 255 };
        int currentH = map->grid[y][x].height;
        // Check East Neighbor
        if (x + 1 < map->columns) {
                int targetH = map->grid[y][x+1].height;
                if (targetH < currentH) {
                    // OPTIMIZATION: If the neighbor is only 1 height lower,
                    // the wall is small. If it's 100 lower, it's huge.
                    // Instead of using 'g' (ground), create 'n' (neighbor top)
                    float neighborH = map->grid[y][x+1].height * 8.0f;
                    Vector2 n2 = { g2.x, g2.y - neighborH };
                    Vector2 n3 = { g3.x, g3.y - neighborH };

                    // Draw only the slice that is visible!
                    DrawTriangleFan((Vector2[]){ t3, n3, n2, t2}, 4, sideL);
                    // DrawTriangleFan((Vector2[]){ t3, g3, g2, t2}, 4, sideL);
                }
            }

        // Check South Neighbor
        if (y + 1 < map->rows) {
            if (map->grid[y+1][x].height < map->grid[y][x].height) {
                float neighborH = map->grid[y+1][x].height * 8.0f;
                Vector2 n4 = { g4.x, g4.y - neighborH };
                Vector2 n3 = { g3.x, g3.y - neighborH };
                DrawTriangleFan((Vector2[]){ t4, n4, n3, t3}, 4, sideR);
            }
        }
    }

    // 1. Draw the "Border" first (slightly offset or larger)
    // Using g1-g4 (ground points) but at height h


    if (map->grid[y][x].type == TILE_WATER) {
        float pulse = sinf(GetTime()) * 20.0f;
            Color waterColor = COLOR_BEAVIS_SHIRT;
            waterColor.a = 100 + (unsigned char)pulse; // Lowered from 150
            DrawTriangleFan((Vector2[]){ t1, t4, t3, t2}, 4, waterColor);
    } else {
        // Color e = base
        Color gridColor = ColorBrightness(base, -0.2f);
        DrawTriangleFan((Vector2[]){ t1, t4, t3, t2 }, 4, gridColor);

        // 2. Draw the "Face" slightly inset
        float inset = 0.2f;
        Vector2 it1 = { t1.x, t1.y + inset };
        Vector2 it2 = { t2.x - inset, t2.y };
        Vector2 it3 = { t3.x, t3.y - inset };
        Vector2 it4 = { t4.x + inset, t4.y };
        DrawTriangleFan((Vector2[]){ it1, it4, it3, it2 }, 4, base);
    }
}
void DrawSimpleSword(Vector2 position, float rotation) {
    float bladeLength = 34.0f;
    float tipLength = 12.0f;
    float bladeWidth = 4.5f;
    float guardWidth = 14.0f;
    float guardThick = 2.5f;
    float hiltLength = 14.0f;

    // METALLIC PALETTE - Using Sunken Ink as the base
    Color metalShadow = COLOR_SUNKEN_INK;
    // Light side is Sunken Ink lightened up significantly
    Color metalBase = ColorBrightness(COLOR_SUNKEN_INK, 0.4f);
    Color highlight = WHITE;
    Color gold = COLOR_PYRITE_BRASS;
    Color leather = COLOR_BURNT_SIENNA;

    float cosR = cosf(rotation);
    float sinR = sinf(rotation);
    float cosP = cosf(rotation + PI/2.0f);
    float sinP = sinf(rotation + PI/2.0f);

    // HILT & POMMEL
    Vector2 hiltEnd = { position.x - cosR * hiltLength, position.y - sinR * hiltLength };
    DrawLineEx(position, hiltEnd, 3.0f, leather);
    DrawCircleV(hiltEnd, 2.5f, gold);

    // THE GUARD
    Vector2 gL = { position.x - cosP * guardWidth/2, position.y - sinP * guardWidth/2 };
    Vector2 gR = { position.x + cosP * guardWidth/2, position.y + sinP * guardWidth/2 };
    DrawLineEx(gL, gR, guardThick, gold);

    // BLADE POINTS
    Vector2 bL = { position.x - cosP * bladeWidth/2, position.y - sinP * bladeWidth/2 };
    Vector2 bR = { position.x + cosP * bladeWidth/2, position.y + sinP * bladeWidth/2 };
    Vector2 sL = { bL.x + cosR * bladeLength, bL.y + sinR * bladeLength };
    Vector2 sR = { bR.x + cosR * bladeLength, bR.y + sinR * bladeLength };
    Vector2 tip = { position.x + cosR * (bladeLength + tipLength), position.y + sinR * (bladeLength + tipLength) };

    // BLADE GEOMETRY
    DrawTriangle(sL, bL, position, metalShadow);
    DrawTriangle(tip, sL, position, metalShadow);
    DrawTriangle(bR, sR, position, metalBase);
    DrawTriangle(sR, tip, position, metalBase);

    // FINISHING SHINE
    DrawLineV(position, tip, highlight);
}

void Draw_MapEntity(MapEntity* entity, Map* map) {
    Vector2 position = GetWorldToIso(entity->position);
    position.y -= entity->altitude;

    if(entity->type == ENTITY_MINERAL){
        DrawMineral(entity->id, position);
    }else{
        Texture2D* sprite = GetSprite(entity->type, entity->id);
        float renderHeight = (entity->type == ENTITY_ITEM) ? (sprite->height * 0.5f) : (float)sprite->height;
        float renderWidth = (entity->type == ENTITY_ITEM) ? (sprite->width * 0.5f) : (float)sprite->width;
        Vector2 drawPos = { position.x - (renderWidth / 2), position.y - renderHeight };


            if(entity->type == ENTITY_ITEM ) {
            DrawTextureEx(*sprite, drawPos, 0.0, 0.5, WHITE);
        } else {
            DrawTextureV(*sprite, drawPos, WHITE);

            // SHADOW LOGIC WITH BOUNDS CHECK
            int tx = (int)(entity->position.x / TILE_SIZE);
            int ty = (int)(entity->position.y / TILE_SIZE);

            float floorY = 0;
            // SAFETY: Prevent accessing grid[-1] or grid[MAX]
            if (tx >= 0 && tx < map->columns && ty >= 0 && ty < map->rows) {
                floorY = map->grid[ty][tx].height * 8.0f;
            }

            Vector2 shadowPos = GetWorldToIso(entity->position);
            shadowPos.y -= floorY;
            DrawCircleGradient(shadowPos.x, shadowPos.y, 8, Fade(BLACK, 0.3f), BLANK);
        }

            if(entity->type == ENTITY_PLAYER) {
                // Adjusted offsets to align with the character's right arm (viewer's left-ish)
                // Based on image_0f13da.png, his 'hand' area is about 32 pixels down and 8 pixels in.
                Vector2 handPos = { drawPos.x + 12, drawPos.y + 48 };

                // For a 'Combat Idle', we can make it breathe slightly
                float idleSwing = sinf(GetTime() * 2.0f) * 0.1f;

                // -0.8f points it up and away; adding idleSwing makes it move!
                DrawSimpleSword(handPos, -0.8f + idleSwing);
            }
    }


}

void DrawWaterEffects(Map* map, int x, int y) {
    if (x < 0 || x >= map->columns || y < 0 || y >= map->rows) return;

    float h = map->grid[y][x].height * 8.0f;
    Vector2 g1 = map->grid[y][x].isoPos;
    Vector2 t1 = { g1.x, g1.y - h };

    // 1. The Sparkle Logic (ONLY the effects)
    float tileSeed = (float)(x * 12.9898f + y * 78.233f);
    float sparkleTime = sinf(GetTime() * 2.5f + tileSeed);

    if (sparkleTime > 0.97f) {
        float offsetX = fmodf(tileSeed * 43758.5453f, (float)TILE_SIZE);
        float offsetY = fmodf(tileSeed * 12345.6789f, (float)TILE_SIZE / 2.0f);

        // Adjusting sparklePos to stay within the diamond
        Vector2 sparklePos = { t1.x + offsetX - (int)(TILE_SIZE/2), t1.y + offsetY };
        float sizePulse = (sinf(GetTime() * 8.0f + tileSeed) + 1.0f) * 1.5f + 1.0f;

        // Instead of just blue, pick a "glitter
        DrawSimpleSparkle(sparklePos, COLOR_INDANTHRONE_BLUE, sizePulse);
        // DrawCircleV(sparklePos, sizePulse * 0.3f, WHITE);
    }
}

void DrawSimpleSparkle(Vector2 pos, Color color, float size) {
    // 1. THE HALO (Very faint, very wide)
    // This hides the 'edges' of the lines by pre-lighting the area
    DrawCircleV(pos, size * 5.0f, Fade(color, 0.03f));
    DrawCircleV(pos, size * 3.0f, Fade(color, 0.08f));

    // 2. THE FLARE (Softer lines)
    // Instead of one thick line, we draw two:
    // a thick faint one and a thin bright one.
    float thin = size * 0.15f;
    float thick = size * 0.4f;

    // Vertical
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size}, thick, Fade(color, 0.1f));
    DrawLineEx((Vector2){pos.x, pos.y - size}, (Vector2){pos.x, pos.y + size}, thin, Fade(color, 0.4f));

    // Horizontal (Wide)
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y}, (Vector2){pos.x + (size * 0.8f), pos.y}, thick, Fade(color, 0.1f));
    DrawLineEx((Vector2){pos.x - (size * 0.8f), pos.y}, (Vector2){pos.x + (size * 0.8f), pos.y}, thin, Fade(color, 0.4f));

    // 3. THE HOT CORE (Gaussian-style stack)
    // This makes the center feel like it's glowing from within
    DrawCircleV(pos, size * 0.8f, Fade(color, 0.5f));
    DrawCircleV(pos, size * 0.4f, ColorBrightness(color, 0.9f));
    // Force a pure white additive strike at the center
    DrawCircleV(pos, size * 0.3f, (Color){ 255, 255, 255, 255 });

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
  entity->next = map->entities;
  map->entities = entity;
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
