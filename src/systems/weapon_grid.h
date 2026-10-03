#ifndef WEAPON_GRID_H
#define WEAPON_GRID_H

#include "defs/types_systems.h"
#include "defs/types_entities.h"

void InitWeaponGrid(WeaponGrid* grid);
void AddWeapon(WeaponGrid* grid, int weapon_id);
void CalculateWeapon(WeaponGrid* grid, int index);
WeaponSlot *GetActiveWeaponsSlot(WeaponGrid* grid);
void SetActiveWeaponsSlot(WeaponGrid* grid, int index);
void AddMaterial(WeaponGrid* grid, int index, EntityType type, int entity_index, int count);
#endif
