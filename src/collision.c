#include "collision.h"

#include "config.h"
#include "effect.h"

static int positions_overlap(Vec2i a, Vec2i b)
{
    return a.x == b.x && a.y == b.y;
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

            if (positions_overlap(shot->position, enemy->position)) {
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
            game->player.lives -= 1;
        }
    }

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy *enemy = &game->enemies[i];

        if (enemy->active && positions_overlap(enemy->position, game->player.position)) {
            enemy->active = 0;
            game->player.lives -= 1;
        }
    }
}
