#ifndef TYPES_SYSTEMS
#define TYPES_SYSTEMS
#include "defs/constants.h"
#include "defs/types_minerals.h"
#include "raylib.h"
#include <stdint.h>

typedef enum ElementFlags {
    ELEMENT_NONE     = 0,
    ELEMENT_FIRE     = 1 << 0, // 1
    ELEMENT_WATER    = 1 << 1, // 2
    ELEMENT_EARTH    = 1 << 2, // 4
    ELEMENT_AIR      = 1 << 3, // 8
    ELEMENT_PIERCING = 1 << 4, // 16
    ELEMENT_EXPLOSIVE= 1 << 5, // 32
} ElementFlags;

typedef struct Input {
    Vector2 dir;
    Vector2 attack_dir;
    Vector2 mouse;
    uint32_t buttons_pressed;
} Input;

typedef enum ButtonPressed {
    NONE_PRESSED = 0,
    KEY_U_PRESSED = 1 << 0,
    KEY_E_PRESSED = 1 << 1,
    JUMP_PRESSED = 1 << 2,
    KEY_W_PRESSED = 1 << 3,
    KEY_S_PRESSED = 1 << 4,
    ENTER_PRESSED = 1 << 5,
    BACKSPACE_PRESSED = 1 << 6,
    SHIFT_DOWN = 1 << 7, // used for edit
    CONTROL_PRESSED = 1 << 8,
    KEY_M_PRESSED = 1 << 9,
    KEY_N_PRESSED = 1 << 10,
    KEY_P_PRESSED = 1 << 11,
    SHIFT_PRESSED = 1 << 12, // for use in play for press
    KEY_L_PRESSED = 1 << 13,
    KEY_I_PRESSED = 1 << 14,
    MOVEMENT_PRESSED = 1 << 15,
    LEFT_MOUSE_CLICKED = 1 << 16,
    LEFT_MOUSE_DOWN = 1 << 17,
    LEFT_MOUSE_RELEASED = 1 << 18,
    KEY_K_PRESSED = 1 << 19,
    KEY_J_PRESSED = 1 << 20,
    KEY_O_PRESSED = 1 << 21,
    KEY_Z_PRESSED = 1 << 22,
    KEY_X_PRESSED = 1 << 23,
    KEY_C_PRESSED = 1 << 24,
    KEY_B_PRESSED = 1 << 25,
    KEY_G_PRESSED = 1 << 26,
    KEY_T_PRESSED = 1 << 27,
    KEY_F_PRESSED = 1 << 28,
    KEY_A_PRESSED = 1 << 29,
    KEY_D_PRESSED = 1 << 30,
    KEY_V_PRESSED = 1<<31,
} ButtonPressed;

typedef struct Message {
    char character_name[32];
    char text[256];
    int id;
    int nextid;
} Message;

typedef struct ScriptManager {
    Message *active_messsage;
    int count;
    int capacity;
    bool active;
    int currentID;
} ScriptManager;

typedef struct Gear {
    int accessory_ids[MAX_ACCESSORY_SLOTS];
    int tarot_ids[3];
} Gear;
#define MAX_ACTIVE_BUFFS 8

typedef enum {
    STAT_STR,
    STAT_DEF,
    STAT_MAG_OFF,
    STAT_MAG_DEF,
    STAT_SPEED,
    STAT_GEOLOGY,
    STAT_BOTANY,
    STAT_ALCHEMY,
    STAT_AEROBICS,
    STAT_ACCESORY_COUNT,
    STAT_COUNT
} StatType;

typedef struct {
    int id;
    int hpBonus;
    int mpBonus;
    int modifiers[STAT_COUNT]; // Holds +2 Aerobics, +1 Grace, -1 Defense, etc.
    float duration;            // Remaining time in seconds
    bool active;
} ActiveBuff;

typedef struct StatBlock {
    int base[STAT_COUNT];
    int current[STAT_COUNT];
    ActiveBuff buffs[MAX_ACTIVE_BUFFS];
    int max_hp;
    int max_base_hp;
    int current_hp;
    int max_mp, max_base_mp, current_mp;
} StatBlock;

typedef struct BasicStatBlock{
    int hp_bonus,mp_bonus;
    int stat_bonuses[STAT_COUNT];
    int cost;
}BasicStatBlock;

typedef struct PlayerTargeting {
    int target_id;            // The unique ID of the locked entity (-1 if none)
    int potential_targets[8]; // Array of nearby target IDs for tab-targeting
    int potential_count;
    bool locked;
} PlayerTargeting;

typedef struct WeaponLeveling{
    int exp;
    int socket;
}WeaponLeveling;

typedef struct WeaponSlot{
    bool active;
    int weapon_id;
    int minerals[MINERAL_COUNT];
    int plant[100];
    int hp_bonus;
    int mp_bonus;
    int stat_bonuses[STAT_COUNT];
    uint32_t elements;
    uint32_t activeElement;
    int level;
    int exp;
    int next_level_exp;
    int current_socket;
    int max_socket;
}WeaponSlot;

typedef struct WeaponGrid{
    WeaponSlot slots[25];
    int count;
    int activeIndex;
}WeaponGrid;


typedef struct Player {
    int item_inventory[100];
    int mineral_inventory[MINERAL_COUNT];
    int plant_inventory[100];
    int vhs_inventory[50];
    WeaponGrid weapon_grid;
    float speed;
    Texture2D sprite; // How fast we move
    StatBlock stats;
    Gear gear;
    PlayerTargeting targeting;
    // Inside your Player struct or stats definition:
    float damage_cooldown;
    uint8_t recipe_inventory[100];
} Player;
#endif
