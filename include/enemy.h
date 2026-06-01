#ifndef RECCA_ENEMY_H
#define RECCA_ENEMY_H

#include "types.h"

void enemies_clear(Enemy enemies[], int count);
void enemies_spawn(Enemy enemies[], int count, int x, EnemyType type);
void enemies_spawn_boss(Enemy enemies[], int count, EnemyType type);
void enemies_update(Enemy enemies[], int enemy_count, Projectile enemy_shots[], int shot_count, int frame, int move_interval);
int enemies_active_count(const Enemy enemies[], int count);
int enemies_has_boss(const Enemy enemies[], int count);
EnemyType enemy_type_for_wave(int wave, int spawn_index);

#endif
