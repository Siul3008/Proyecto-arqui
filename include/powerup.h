#ifndef RECCA_POWERUP_H
#define RECCA_POWERUP_H

#include "types.h"

void powerups_clear(PowerUp powerups[], int count);
void powerups_spawn_weapon(PowerUp powerups[], int count, Vec2i position, WeaponType weapon);
void powerups_spawn_drone(PowerUp powerups[], int count, Vec2i position);
void powerups_update(PowerUp powerups[], int count, int frame, int move_interval);
WeaponType powerup_weapon_for_seed(int seed);
PowerUpType powerup_type_for_seed(int seed);

#endif
