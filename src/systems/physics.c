#include "systems/physics.h"

bool UpdatePhysics(Map* map, Vector2 dir, bool jump){
  UpdateVelocity(map, dir, jump);
  ApplyGravity(map);
  ApplyFriction(map);
  return ResolveMovement(map);
}

void UpdateVelocity(Map* map, Vector2 dir, bool jump){
  float dt = GetFrameTime();
  if (jump){
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

void ApplyGravity(Map* map) {
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

void ApplyFriction(Map* map){
   if(map->player->velocity.x != 0 || map->player->velocity.y != 0 ){
    map->player->velocity.x *= 0.9f;
    map->player->velocity.y *= 0.9f;
  }
}

bool ResolveMovement(Map* map){
  Vector2 nextX = map->player->position;
  bool moved = false;
  nextX.x += map->player->velocity.x;
  if (!CheckCollision(map, nextX)) {
    map->player->position.x = nextX.x;
    moved = true;
  }

  Vector2 nextY = map->player->position;
  nextY.y += map->player->velocity.y;
  if (!CheckCollision(map, nextY)) {
    map->player->position.y = nextY.y;
    moved = true;
  }

  return moved;
}

bool CheckCollision(Map* map, Vector2 nextPos) {
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
