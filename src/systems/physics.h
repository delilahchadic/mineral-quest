#ifndef PHYSICS_H
#define PHYSICS_H
#include "raylib.h"
#include "environment/map.h"
#include "engine/register.h"

bool UpdatePhysics(Map* map, Vector2 dir, bool jump);
void UpdateVelocity(Map* map, Vector2 dir, bool jump);
void ApplyGravity(Map* map);
bool ResolveMovement(Map* map);
void ApplyFriction(Map* map);
bool CheckCollision(Map* map, Vector2 nextPos);
#endif