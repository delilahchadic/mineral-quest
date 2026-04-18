#ifndef PHYSICS_H
#define PHYSICS_H
#include "raylib.h"
#include "environment/map.h"
#include "engine/register.h"

bool UpdatePhysics(Map* map, const Input* input);
void UpdateVelocity(Map* map, const Input* input);
void ApplyGravity(Map* map);
bool ResolveMovement(Map* map);
bool CheckCollision(Map* map, Vector2 nextPos);
#endif