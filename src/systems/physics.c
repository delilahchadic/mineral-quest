#include "systems/physics.h"

#include "environment/map.h"
#include "registry/register.h"
#include "systems/input.h"
#include "raymath.h"
bool UpdatePhysics(Map* map,const Input* input){
  UpdateVelocity(map, input);
  ApplyGravity(map);

  return ResolveMovement(map);
}

void UpdateVelocity(Map* map, const Input* input) {
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    // Get current floor height to check if we are grounded
    int tx = Clamp((int)(map->player->position.x / TILE_SIZE), 0, map->columns - 1);
    int ty = Clamp((int)(map->player->position.y / TILE_SIZE), 0, map->rows - 1);
    float floorY = map->grid[ty][tx].height * 8.0f;

    // JUMP: If altitude is close to the floor, BLAST OFF.
    if (input->buttons_pressed & JUMP_PRESSED) {
        // Allow jumping if we are on the floor (or within 2 pixels for "coyote time")
        if (map->player->altitude <= floorY + 2.0f) {
            map->player->state = JUMPING_STATE;
            map->player->vertical_velocity = 500.0f;
        }
    }

    if (input->buttons_pressed & MOVEMENT_PRESSED) {
        float length = (input->dir.x != 0 && input->dir.y != 0) ? 0.707f : 1.0f;
        map->player->velocity.x = input->dir.x * PLAYER->speed * length;
        map->player->velocity.y = input->dir.y * PLAYER->speed * length;
    } else {
        map->player->velocity.x = 0;
        map->player->velocity.y = 0;
    }
}

void ApplyGravity(Map* map) {
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    // Get current floor height in pixels
    int tx = (int)(map->player->position.x / TILE_SIZE);
    int ty = (int)(map->player->position.y / TILE_SIZE);
    tx = Clamp(tx, 0, map->columns - 1);
    ty = Clamp(ty, 0, map->rows - 1);

    float currentFloorY = map->grid[ty][tx].height * 8.0f;

    // 1. GRAVITY ALWAYS PULLS DOWN
    const float GRAVITY = 1200.0f;
    map->player->vertical_velocity -= GRAVITY * dt;

    // Move the player's altitude
    map->player->altitude += map->player->vertical_velocity * dt;

    // 2. THE HARD FLOOR
    if (map->player->altitude <= currentFloorY) {
        map->player->altitude = currentFloorY;
        map->player->vertical_velocity = 0;
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
    int cur_x = (int)(map->player->position.x / TILE_SIZE);
    int cur_y = (int)(map->player->position.y / TILE_SIZE);
    if (cur_x < 0 || cur_x >= map->columns || cur_y < 0 || cur_y >= map->rows) return true;

    // The player's actual height is just their altitude
    float currentWorldHeight = map->player->altitude / 8.0f;

    float foot_left   = nextPos.x - 14;
    float foot_right  = nextPos.x + 14;
    float foot_top    = nextPos.y - 8;
    float foot_bottom = nextPos.y + 8;

    int checkX[] = { (int)(foot_left / TILE_SIZE), (int)(foot_right / TILE_SIZE) };
    int checkY[] = { (int)(foot_top / TILE_SIZE), (int)(foot_bottom / TILE_SIZE) };

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float targetHeight = (float)map->grid[checkY[j]][checkX[i]].height;
            bool is_blocking = TILE_REGISTRY[map->grid[checkY[j]][checkX[i]].type].is_blocking;

            if (is_blocking) return true;

            // STEP HEIGHT LOGIC:
            // If the tile is more than 1.5 units higher than our CURRENT feet...
            if (targetHeight > currentWorldHeight + 1.5f) {
                return true; // We hit a wall
            }
        }
    }
    return false;
}
