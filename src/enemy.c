#include "enemy.h"

#include "config.h"
#include "projectile.h"

static int enemy_is_boss_type(EnemyType type)
{
    return type == ENEMY_MINI_BOSS || type == ENEMY_STAGE_BOSS;
}

static void move_boss_horizontal(Enemy *enemy)
{
    int next_x = enemy->position.x + enemy->velocity.x;
    int left_limit = enemy_min_center_x(enemy->type);
    int right_limit = enemy_max_center_x(enemy->type);

    if (enemy->velocity.x == 0) {
        enemy->velocity.x = 1;
        next_x = enemy->position.x + enemy->velocity.x;
    }

    if (next_x <= left_limit) {
        enemy->position.x = left_limit;
        enemy->velocity.x = 1;
    } else if (next_x >= right_limit) {
        enemy->position.x = right_limit;
        enemy->velocity.x = -1;
    } else {
        enemy->position.x = next_x;
    }
}

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
    case ENEMY_MINI_BOSS:
    case ENEMY_STAGE_BOSS:
        move_boss_horizontal(enemy);
        break;
    }
}

static void keep_boss_inside_vertical_bounds(Enemy *enemy)
{
    if (!enemy_is_boss_type(enemy->type)) {
        return;
    }

    if (enemy->position.y < 0 || enemy->position.y >= GAME_HEIGHT) {
        enemy->position.y = 2;
    }

    enemy->velocity.y = 0;
}

static void keep_enemy_inside_horizontal_bounds(Enemy *enemy)
{
    int left_limit = enemy_min_center_x(enemy->type);
    int right_limit = enemy_max_center_x(enemy->type);

    if (enemy->position.x < left_limit || enemy_left(enemy) < 0) {
        enemy->position.x = left_limit;
        enemy->velocity.x = 1;
    } else if (enemy->position.x > right_limit || enemy_right(enemy) > GAME_WIDTH - 1) {
        enemy->position.x = right_limit;
        enemy->velocity.x = -1;
    }
}

static int fire_cooldown_for_type(EnemyType type, int slot)
{
    switch (type) {
    case ENEMY_STRAIGHT:
        return 18 + (slot % 4);
    case ENEMY_DIAGONAL:
        return 15 + (slot % 5);
    case ENEMY_ZIGZAG:
        return 13 + (slot % 4);
    case ENEMY_FAST:
        return 18 + (slot % 6);
    case ENEMY_MINI_BOSS:
        return 12;
    case ENEMY_STAGE_BOSS:
        return 9;
    }

    return 18;
}

static void fire_mini_boss_pattern(const Enemy *enemy, Projectile enemy_shots[], int shot_count)
{
    Vec2i center = {enemy->position.x, enemy->position.y + 1};
    Vec2i left = {enemy->position.x - 1, enemy->position.y + 1};
    Vec2i right = {enemy->position.x + 1, enemy->position.y + 1};

    if ((enemy->age / 40) % 2 == 0) {
        projectiles_spawn(enemy_shots, shot_count, left, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, right, (Vec2i){0, 1});
    } else {
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){-1, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){1, 1});
    }
}

static void fire_stage_boss_pattern(const Enemy *enemy, Projectile enemy_shots[], int shot_count)
{
    Vec2i center = {enemy->position.x, enemy->position.y + 1};
    Vec2i far_left = {enemy->position.x - 2, enemy->position.y + 1};
    Vec2i left = {enemy->position.x - 1, enemy->position.y + 1};
    Vec2i right = {enemy->position.x + 1, enemy->position.y + 1};
    Vec2i far_right = {enemy->position.x + 2, enemy->position.y + 1};
    int pattern = (enemy->age / 50) % 3;

    if (pattern == 0) {
        projectiles_spawn(enemy_shots, shot_count, far_left, (Vec2i){-1, 1});
        projectiles_spawn(enemy_shots, shot_count, left, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, right, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, far_right, (Vec2i){1, 1});
    } else if (pattern == 1) {
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){-1, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){1, 1});
    } else {
        projectiles_spawn(enemy_shots, shot_count, far_left, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, far_right, (Vec2i){0, 1});
    }
}

int enemy_max_health(EnemyType type)
{
    switch (type) {
    case ENEMY_MINI_BOSS:
        return MINI_BOSS_HEALTH;
    case ENEMY_STAGE_BOSS:
        return STAGE_BOSS_HEALTH;
    default:
        return 1;
    }
}

int enemy_hitbox_half_width(EnemyType type)
{
    switch (type) {
    case ENEMY_MINI_BOSS:
        return 2;
    case ENEMY_STAGE_BOSS:
        return 3;
    default:
        return 0;
    }
}

int enemy_min_center_x(EnemyType type)
{
    return enemy_hitbox_half_width(type);
}

int enemy_max_center_x(EnemyType type)
{
    return GAME_WIDTH - 1 - enemy_hitbox_half_width(type);
}

int enemy_left(const Enemy *enemy)
{
    return enemy->position.x - enemy_hitbox_half_width(enemy->type);
}

int enemy_right(const Enemy *enemy)
{
    return enemy->position.x + enemy_hitbox_half_width(enemy->type);
}

int enemy_top(const Enemy *enemy)
{
    return enemy->position.y;
}

int enemy_bottom(const Enemy *enemy)
{
    return enemy->position.y;
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
            enemies[i].health = enemy_max_health(type);
            enemies[i].fire_cooldown = fire_cooldown_for_type(type, i);
            enemies[i].age = 0;
            enemies[i].type = type;
            return;
        }
    }
}

void enemies_spawn_boss(Enemy enemies[], int count, EnemyType type)
{
    for (int i = 0; i < count; ++i) {
        if (!enemies[i].active) {
            enemies[i].active = 1;
            enemies[i].position.x = GAME_WIDTH / 2;
            enemies[i].position.y = 2;
            enemies[i].velocity.x = 1;
            enemies[i].velocity.y = 0;
            enemies[i].health = enemy_max_health(type);
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

        keep_enemy_inside_horizontal_bounds(enemy);
        keep_boss_inside_vertical_bounds(enemy);

        if (enemy->position.y >= GAME_HEIGHT &&
            !enemy_is_boss_type(enemy->type)) {
            enemy->active = 0;
            continue;
        }

        if (enemy->fire_cooldown > 0) {
            enemy->fire_cooldown -= 1;
        } else {
            if (enemy->type == ENEMY_STAGE_BOSS) {
                fire_stage_boss_pattern(enemy, enemy_shots, shot_count);
            } else if (enemy->type == ENEMY_MINI_BOSS) {
                fire_mini_boss_pattern(enemy, enemy_shots, shot_count);
            } else {
                Vec2i shot_position = {enemy->position.x, enemy->position.y + 1};
                Vec2i shot_velocity = {0, 1};
                projectiles_spawn(enemy_shots, shot_count, shot_position, shot_velocity);
            }
            enemy->fire_cooldown = fire_cooldown_for_type(enemy->type, i);
        }
    }
}

const Enemy *enemies_find_boss(const Enemy enemies[], int count)
{
    for (int i = 0; i < count; ++i) {
        if (enemies[i].active && enemy_is_boss_type(enemies[i].type)) {
            return &enemies[i];
        }
    }

    return 0;
}

int enemies_has_boss(const Enemy enemies[], int count)
{
    for (int i = 0; i < count; ++i) {
        if (enemies[i].active && enemy_is_boss_type(enemies[i].type)) {
            return 1;
        }
    }

    return 0;
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
