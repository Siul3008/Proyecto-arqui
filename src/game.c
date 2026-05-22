#include "game.h"

#include "collision.h"
#include "enemy.h"
#include "player.h"
#include "projectile.h"

void game_init(GameState *game)
{
    player_init(&game->player);
    projectiles_clear(game->player_shots, MAX_PLAYER_SHOTS);
    projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);
    enemies_clear(game->enemies, MAX_ENEMIES);

    game->frame = 0;
    game->running = 1;
    game->game_over = 0;
}

void game_update(GameState *game, int input_mask)
{
    if (input_mask & INPUT_QUIT) {
        game->running = 0;
        return;
    }

    player_update(&game->player, input_mask, game->player_shots, MAX_PLAYER_SHOTS);

    if (game->frame % 14 == 0) {
        int spawn_x = 3 + ((game->frame * 7) % (GAME_WIDTH - 6));
        enemies_spawn(game->enemies, MAX_ENEMIES, spawn_x);
    }

    projectiles_update(game->player_shots, MAX_PLAYER_SHOTS);
    projectiles_update(game->enemy_shots, MAX_ENEMY_SHOTS);
    enemies_update(game->enemies, MAX_ENEMIES, game->enemy_shots, MAX_ENEMY_SHOTS, game->frame);
    collisions_update(game);

    game->frame += 1;
}
