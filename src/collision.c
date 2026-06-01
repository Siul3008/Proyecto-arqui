#include "collision.h"

#include "config.h"
#include "effect.h"
#include "enemy.h"

static int positions_overlap(Vec2i a, Vec2i b)
{
    return a.x == b.x && a.y == b.y;
}

static int ranges_overlap(int center_a, int half_a, int center_b, int half_b)
{
    return center_a - half_a <= center_b + half_b &&
           center_b - half_b <= center_a + half_a;
}

static int projectile_hits_enemy(Vec2i shot_position, const Enemy *enemy)
{
    return shot_position.y == enemy->position.y &&
           ranges_overlap(shot_position.x, 0, enemy->position.x, enemy_hitbox_half_width(enemy->type));
}

static int player_hits_enemy(const Player *player, const Enemy *enemy)
{
    return player->position.y == enemy->position.y &&
           ranges_overlap(player->position.x, 1, enemy->position.x, enemy_hitbox_half_width(enemy->type));
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
}
