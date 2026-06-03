#include "collision.h"

#include "config.h"
#include "effect.h"
#include "enemy.h"
#include "powerup.h"

static int positions_overlap(Vec2i a, Vec2i b)
{
    return a.x == b.x && a.y == b.y;
}

static int ranges_overlap(int left_a, int right_a, int left_b, int right_b)
{
    return left_a <= right_b && left_b <= right_a;
}

static int projectile_hits_enemy(Vec2i shot_position, const Enemy *enemy)
{
    return shot_position.y >= enemy_top(enemy) &&
           shot_position.y <= enemy_bottom(enemy) &&
           shot_position.x >= enemy_left(enemy) &&
           shot_position.x <= enemy_right(enemy);
}

static int player_hits_enemy(const Player *player, const Enemy *enemy)
{
    return ranges_overlap(player->position.y, player->position.y, enemy_top(enemy), enemy_bottom(enemy)) &&
           ranges_overlap(player->position.x - 1, player->position.x + 1, enemy_left(enemy), enemy_right(enemy));
}

static int score_for_enemy_type(EnemyType type)
{
    switch (type) {
    case ENEMY_MINI_BOSS:
        return MINI_BOSS_SCORE;
    case ENEMY_STAGE_BOSS:
        return STAGE_BOSS_SCORE;
    default:
        return 100;
    }
}

static int enemy_should_drop_powerup(const GameState *game, const Enemy *enemy)
{
    if (enemy->type == ENEMY_MINI_BOSS || enemy->type == ENEMY_STAGE_BOSS) {
        return 1;
    }

    return ((game->frame + enemy->position.x + game->player.score) % POWERUP_DROP_DIVISOR) == 0;
}

static void drop_powerup_from_enemy(GameState *game, const Enemy *enemy)
{
    if (!enemy_should_drop_powerup(game, enemy)) {
        return;
    }

    int seed = game->frame + game->player.score + enemy->position.x + enemy->position.y;
    if (powerup_type_for_seed(seed) == POWERUP_DRONE) {
        powerups_spawn_drone(game->powerups, MAX_POWERUPS, enemy->position);
    } else {
        powerups_spawn_weapon(game->powerups,
                              MAX_POWERUPS,
                              enemy->position,
                              powerup_weapon_for_seed(seed));
    }
}

static void damage_player(Player *player)
{
    if (player->invulnerable_timer > 0) {
        return;
    }

    player->lives -= 1;
    player->invulnerable_timer = PLAYER_INVULNERABLE_FRAMES;
}

void collisions_update(GameState *game)
{
    for (int i = 0; i < MAX_PLAYER_SHOTS; ++i) {
        Projectile *shot = &game->player_shots[i];

        if (!shot->active) {
            continue;
        }

        for (int j = 0; j < MAX_ENEMIES; ++j) {
            Enemy *enemy = &game->enemies[j];

            if (!enemy->active) {
                continue;
            }

            if (projectile_hits_enemy(shot->position, enemy)) {
                shot->active = 0;
                enemy->health -= 1;
                if (enemy->health <= 0) {
                    effect_spawn(game->effects, MAX_EFFECTS, enemy->position, EXPLOSION_DURATION);
                    drop_powerup_from_enemy(game, enemy);
                    enemy->active = 0;
                    game->player.score += score_for_enemy_type(enemy->type);
                }
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        Projectile *shot = &game->enemy_shots[i];

        if (!shot->active) {
            continue;
        }

        if (positions_overlap(shot->position, game->player.position)) {
            shot->active = 0;
            damage_player(&game->player);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy *enemy = &game->enemies[i];

        if (enemy->active && player_hits_enemy(&game->player, enemy)) {
            if (enemy_hitbox_half_width(enemy->type) == 0) {
                enemy->active = 0;
            }
            damage_player(&game->player);
        }
    }

    for (int i = 0; i < MAX_POWERUPS; ++i) {
        PowerUp *powerup = &game->powerups[i];

        if (powerup->active && positions_overlap(powerup->position, game->player.position)) {
            if (powerup->type == POWERUP_DRONE) {
                game->player.drone_count = MAX_PLAYER_DRONES;
                game->player.drone_timer = PLAYER_DRONE_DURATION_FRAMES;
            } else {
                game->player.weapon = powerup->weapon;
                game->player.weapon_timer = PLAYER_WEAPON_DURATION_FRAMES;
            }
            powerup->active = 0;
        }
    }
}
