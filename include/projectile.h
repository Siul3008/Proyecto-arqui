#ifndef RECCA_PROJECTILE_H
#define RECCA_PROJECTILE_H

#include "types.h"

void projectiles_clear(Projectile projectiles[], int count);
void projectiles_spawn(Projectile projectiles[], int count, Vec2i position, Vec2i velocity);
void projectiles_update(Projectile projectiles[], int count, int frame, int move_interval);

#endif
