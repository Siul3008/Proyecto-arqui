#ifndef RECCA_ENEMY_H
#define RECCA_ENEMY_H

#include "types.h"

void enemies_clear(Enemy enemies[], int count);
void enemies_spawn(Enemy enemies[], int count, int x);
void enemies_update(Enemy enemies[], int enemy_count, Projectile enemy_shots[], int shot_count, int frame);

#endif
