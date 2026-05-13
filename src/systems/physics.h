#ifndef PHYSICS_H
#define PHYSICS_H

#include "defs/types_env.h"
#include "defs/types_systems.h"
#include "raylib.h"

bool UpdatePhysics(Map* map, const Input* input);
void UpdateVelocity(Map* map, const Input* input);
void ApplyGravity(Map* map);
bool ResolveMovement(Map* map);
bool CheckCollision(Map* map, Vector2 nextPos);
#endif
