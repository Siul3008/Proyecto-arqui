#include "collision.h"

#include "config.h"

static int positions_overlap(Vec2i a, Vec2i b)
{
    return a.x == b.x && a.y == b.y;
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
                    enemy->active = 0;
                    game->player.score += 100;
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

    if (game->player.lives <= 0) {
        game->game_over = 1;
        game->running = 0;
    }
}
