#include "enemy.h"

#include "config.h"
#include "projectile.h"

void enemies_clear(Enemy enemies[], int count)
{
    for (int i = 0; i < count; ++i) {
        enemies[i].active = 0;
        enemies[i].position.x = 0;
        enemies[i].position.y = 0;
        enemies[i].velocity.x = 0;
        enemies[i].velocity.y = 1;
        enemies[i].health = 1;
        enemies[i].fire_cooldown = 0;
    }
}

void enemies_spawn(Enemy enemies[], int count, int x)
{
    for (int i = 0; i < count; ++i) {
        if (!enemies[i].active) {
            enemies[i].active = 1;
            enemies[i].position.x = x;
            enemies[i].position.y = 0;
            enemies[i].velocity.x = (x % 3) - 1;
            enemies[i].velocity.y = 1;
            enemies[i].health = 1;
            enemies[i].fire_cooldown = 8 + (x % 5);
            return;
        }
    }
}

void enemies_update(Enemy enemies[], int enemy_count, Projectile enemy_shots[], int shot_count, int frame)
{
    for (int i = 0; i < enemy_count; ++i) {
        Enemy *enemy = &enemies[i];

        if (!enemy->active) {
            continue;
        }

        if (frame % 2 == 0) {
            enemy->position.x += enemy->velocity.x;
            enemy->position.y += enemy->velocity.y;
        }

        if (enemy->position.x <= 0 || enemy->position.x >= GAME_WIDTH - 1) {
            enemy->velocity.x *= -1;
        }

        if (enemy->position.y >= GAME_HEIGHT) {
            enemy->active = 0;
            continue;
        }

        if (enemy->fire_cooldown > 0) {
            enemy->fire_cooldown -= 1;
        } else {
            Vec2i shot_position = {enemy->position.x, enemy->position.y + 1};
            Vec2i shot_velocity = {0, 1};
            projectiles_spawn(enemy_shots, shot_count, shot_position, shot_velocity);
            enemy->fire_cooldown = 12 + (i % 6);
        }
    }
}
