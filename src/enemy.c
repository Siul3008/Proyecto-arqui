#include "enemy.h"

#include "config.h"
#include "projectile.h"

static void move_enemy(Enemy *enemy)
{
    switch (enemy->type) {
    case ENEMY_STRAIGHT:
        enemy->position.y += enemy->velocity.y;
        break;
    case ENEMY_DIAGONAL:
        enemy->position.x += enemy->velocity.x;
        enemy->position.y += enemy->velocity.y;
        break;
    case ENEMY_ZIGZAG:
        if (enemy->age % 10 == 0) {
            enemy->velocity.x *= -1;
        }
        enemy->position.x += enemy->velocity.x;
        enemy->position.y += enemy->velocity.y;
        break;
    case ENEMY_FAST:
        enemy->position.x += enemy->velocity.x;
        enemy->position.y += enemy->velocity.y + 1;
        break;
    }
}

static int fire_cooldown_for_type(EnemyType type, int slot)
{
    switch (type) {
    case ENEMY_STRAIGHT:
        return 18 + (slot % 4);
    case ENEMY_DIAGONAL:
        return 14 + (slot % 5);
    case ENEMY_ZIGZAG:
        return 12 + (slot % 4);
    case ENEMY_FAST:
        return 20 + (slot % 6);
    }

    return 18;
}

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
        enemies[i].age = 0;
        enemies[i].type = ENEMY_STRAIGHT;
    }
}

void enemies_spawn(Enemy enemies[], int count, int x, EnemyType type)
{
    for (int i = 0; i < count; ++i) {
        if (!enemies[i].active) {
            enemies[i].active = 1;
            enemies[i].position.x = x;
            enemies[i].position.y = 0;
            enemies[i].velocity.x = ((x + i) % 2 == 0) ? 1 : -1;
            enemies[i].velocity.y = 1;
            enemies[i].health = 1;
            enemies[i].fire_cooldown = fire_cooldown_for_type(type, i);
            enemies[i].age = 0;
            enemies[i].type = type;
            return;
        }
    }
}

void enemies_update(Enemy enemies[], int enemy_count, Projectile enemy_shots[], int shot_count, int frame, int move_interval)
{
    for (int i = 0; i < enemy_count; ++i) {
        Enemy *enemy = &enemies[i];

        if (!enemy->active) {
            continue;
        }

        if (move_interval <= 0 || frame % move_interval == 0) {
            move_enemy(enemy);
        }
        enemy->age += 1;

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
            enemy->fire_cooldown = fire_cooldown_for_type(enemy->type, i);
        }
    }
}

int enemies_active_count(const Enemy enemies[], int count)
{
    int active_count = 0;

    for (int i = 0; i < count; ++i) {
        if (enemies[i].active) {
            active_count += 1;
        }
    }

    return active_count;
}

EnemyType enemy_type_for_wave(int wave, int spawn_index)
{
    if (wave <= 1) {
        return ENEMY_STRAIGHT;
    }

    if (wave == 2) {
        return (spawn_index % 2 == 0) ? ENEMY_STRAIGHT : ENEMY_DIAGONAL;
    }

    if (wave == 3) {
        return (spawn_index % 3 == 0) ? ENEMY_ZIGZAG : ENEMY_DIAGONAL;
    }

    switch (spawn_index % 4) {
    case 0:
        return ENEMY_FAST;
    case 1:
        return ENEMY_ZIGZAG;
    case 2:
        return ENEMY_DIAGONAL;
    default:
        return ENEMY_STRAIGHT;
    }
}
