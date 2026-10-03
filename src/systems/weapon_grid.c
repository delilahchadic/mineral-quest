#include "systems/weapon_grid.h"
#include "defs/types_entities.h"
#include "defs/types_minerals.h"
#include "defs/types_systems.h"
#include "registry/register.h"
#include "registry/weapon_register.h"
#include <stddef.h>
void InitWeaponGrid(WeaponGrid* grid){

    AddWeapon(grid, 12);
    grid->activeIndex = 0;
    CalculateWeapon(grid, 0);
}

WeaponSlot *GetActiveWeaponsSlot(WeaponGrid* grid){
    if(grid->activeIndex == -1 || grid->count < 1) return NULL;
    return &grid->slots[grid->activeIndex];
}

void SetActiveWeaponsSlot(WeaponGrid* grid, int index){
    if( grid->count < 1 || !grid->slots[index].active || index < 0 || index >24) return;
    grid->activeIndex = index;
    return;
}

void AddWeapon(WeaponGrid* grid, int weapon_id){
    if(grid->count>=25) return; //undefined for now. for now we dont expect there to be 25 items on the test map or we can assume this
    WeaponSlot* nextSlot= NULL;
    int weaponIndex = -1;
    for(int i=0;i<25;i++){
        if(!grid->slots[i].active){
            nextSlot = &grid->slots[i];
            weaponIndex = i;
            break;
        }
    }
    if(!nextSlot) return;
    nextSlot->active=true;
    nextSlot->weapon_id = weapon_id;
    nextSlot->level = 1;
    WeaponLeveling leveling = GetWeaponLeveling(weapon_id);
    nextSlot->next_level_exp = leveling.exp;
    nextSlot->max_socket = leveling.socket;
    CalculateWeapon(grid, weaponIndex);
    grid->count++;
    return;
}

void AddMaterial(WeaponGrid* grid, int index, EntityType type, int entity_index, int count){
    WeaponSlot *slot = &grid->slots[index];
    if(slot->current_socket + GetCost(type, entity_index) <= (slot->max_socket)){

        switch (type) {
            case ENTITY_PLANT:
                slot->current_socket += GetCost(type, entity_index);
                slot->plant[entity_index] += count;
                return;
            case ENTITY_MINERAL:
                slot->current_socket += GetCost(type, entity_index);
                slot->minerals[entity_index] += count;
                return;
            default: return;
        }
    }

}

void CalculateWeapon(WeaponGrid* grid, int index){

    WeaponSlot* slot = &grid->slots[index];
    if(!slot->active) return;
    ItemDefinition *weapon = &ITEM_REGISTRY[slot->weapon_id];
    slot->hp_bonus= weapon->hp_bonus;
    slot->mp_bonus=weapon->mp_bonus;

    for(int i=0;i<STAT_COUNT;i++){
        slot->stat_bonuses[i]=weapon->stat_bonuses[i];
    }

    for(int i = 0;i< GetEntityTypeCount(ENTITY_PLANT);i++){
        if(slot->plant[i] > 0){
            Plant *plant = &PLANT_REGISTRY[i];
            slot->hp_bonus += slot->plant[i] * plant->hp_bonus;
            slot->mp_bonus += slot->plant[i] * plant->mp_bonus;
            for(int j=0;j< STAT_COUNT;j++){
                slot->stat_bonuses[j] += slot->plant[i] * plant->stat_bonuses[j];
            }
        }
    }

    for(int i = 0;i< MINERAL_COUNT;i++){
        if(slot->minerals[i] > 0){
            BasicStatBlock *mineral = &MINERAL_STATS[i];
            slot->hp_bonus += slot->minerals[i] * mineral->hp_bonus;
            slot->mp_bonus += slot->minerals[i] * mineral->mp_bonus;
            for(int j=0;j< STAT_COUNT;j++){
                slot->stat_bonuses[j] += slot->minerals[i] * mineral->stat_bonuses[j];
            }
        }
    }
}
