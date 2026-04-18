#include "systems/physics.h"
#include "systems/input.h"

bool UpdatePhysics(Map* map,const Input* input){
  UpdateVelocity(map, input);
  ApplyGravity(map);

  return ResolveMovement(map);
}

void UpdateVelocity(Map* map,const Input* input){
  float dt = GetFrameTime();
  if (dt > 0.1f) dt = 0.1f;
  if (input->buttons_pressed & JUMP_PRESSED){
    if(map->player->state != JUMPING_STATE){
      map->player->state = JUMPING_STATE;
      map->player->vertical_velocity= 8.0f;
    }
  }
  
  if (input->buttons_pressed & MOVEMENT_PRESSED) {
    // This is the "proper" way to get 0.707 for diagonals
    float length = (input->dir.x != 0 && input->dir.y != 0) ? 0.707f : 1.0f;
    map->player->velocity.x = input->dir.x * PLAYER->speed * length;
    map->player->velocity.y = input->dir.y * PLAYER->speed * length;
  } else{
    map->player->velocity.x = 0;
    map->player->velocity.y = 0; 
  }
}

void ApplyGravity(Map* map) {
  int tx = (int)(map->player->position.x / TILE_SIZE);
  int ty = (int)(map->player->position.y / TILE_SIZE);
  if (tx < 0 || tx >= map->columns || ty < 0 || ty >= map->rows) return;

  // If we move to a lower tile, the "jumpoffset" needs to increase 
  // to keep us at the same visual height while we start falling.
  int current_tile_height = map->grid[ty][tx].height;

  if (map->lastTileHeight != -1 && current_tile_height < map->lastTileHeight) {
      // We just stepped down! Add the difference to jumpoffset so we don't "snap" down.
      map->player->jumpoffset += (map->lastTileHeight - current_tile_height) * 8.0f;
      map->player->state = JUMPING_STATE;
  }
  map->lastTileHeight = current_tile_height;

  // Standard Gravity Logic
  if (map->player->state == JUMPING_STATE || map->player->jumpoffset > 0.0f) {
      map->player->vertical_velocity -= 1.0f; 
      map->player->jumpoffset += map->player->vertical_velocity;
  }

  // Hit the floor
  if (map->player->jumpoffset <= 0.0f) {
      map->player->jumpoffset = 0.0f;
      map->player->vertical_velocity = 0.0f;
      map->player->state = NORMAL_STATE;
  }
}

bool ResolveMovement(Map* map){

  float dt = GetFrameTime();
  if (dt > 0.1f) dt = 0.1f; // Keep your clamp!
  Vector2 nextX = map->player->position;
  bool moved = false;
  nextX.x += map->player->velocity.x * dt;
  if (!CheckCollision(map, nextX)) {
    map->player->position.x = nextX.x;
    moved = true;
  }

  Vector2 nextY = map->player->position;
  nextY.y += map->player->velocity.y * dt;
  if (!CheckCollision(map, nextY)) {
    map->player->position.y = nextY.y;
    moved = true;
  }

  return moved;
}

bool CheckCollision(Map* map, Vector2 nextPos) {
    // 1. Get the tile coordinates for the CURRENT position
    int cur_x = (int)(map->player->position.x / TILE_SIZE);
    int cur_y = (int)(map->player->position.y / TILE_SIZE);
    
    // Safety check
    if (cur_x < 0 || cur_x >= map->columns || cur_y < 0 || cur_y >= map->rows) return true;

    // IMPORTANT: Calculate current world height (Tile + Jump)
    float currentWorldHeight = (float)map->grid[cur_y][cur_x].height + (map->player->jumpoffset / 8.0f);

    // Collision Box
    float foot_left   = nextPos.x - 14;
    float foot_right  = nextPos.x + 14;
    float foot_top    = nextPos.y - 8;
    float foot_bottom = nextPos.y + 8;

    if (foot_left < 0 || foot_right >= map->pixel_width || foot_top < 0 || foot_bottom >= map->pixel_height) return true;

    int checkX[] = { (int)(foot_left / TILE_SIZE), (int)(foot_right / TILE_SIZE) };
    int checkY[] = { (int)(foot_top / TILE_SIZE), (int)(foot_bottom / TILE_SIZE) };

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float targetHeight = (float)map->grid[checkY[j]][checkX[i]].height;
            bool is_blocking = TILE_REGISTRY[map->grid[checkY[j]][checkX[i]].type].is_blocking;

            // Use 1.1f to allow for tiny float errors when stepping up 1 unit
            if (is_blocking || targetHeight > (currentWorldHeight + 1.1f)) {
                return true; 
            }
        }
    }
    return false;
}
