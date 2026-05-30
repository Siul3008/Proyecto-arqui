#include "game.h"

#include "collision.h"
#include "enemy.h"
#include "player.h"
#include "projectile.h"
#include "effect.h"

static int enemies_to_spawn_for_wave(int wave)
{
    return WAVE_BASE_ENEMY_COUNT + (wave - 1) * 2;
}

static int spawn_x_for_wave(const GameState *game)
{
    int usable_width = GAME_WIDTH - 6;
    return 3 + ((game->frame * 7 + game->wave_spawned * 11) % usable_width);
}

static int enemy_move_interval_for_wave(int wave)
{
    if (wave <= 1) {
        return ENEMY_BASE_MOVE_INTERVAL;
    }
    if (wave <= 3) {
        return ENEMY_BASE_MOVE_INTERVAL - 1;
    }

    return 1;
}

static int enemy_shot_move_interval_for_wave(int wave)
{
    if (wave <= 2) {
        return ENEMY_SHOT_BASE_MOVE_INTERVAL;
    }

    return 1;
}

static void update_wave_spawning(GameState *game)
{
    int enemies_in_wave = enemies_to_spawn_for_wave(game->wave);

    if (game->wave_spawned < enemies_in_wave && game->frame >= game->next_spawn_frame) {
        EnemyType type = enemy_type_for_wave(game->wave, game->wave_spawned);
        enemies_spawn(game->enemies, MAX_ENEMIES, spawn_x_for_wave(game), type);
        game->wave_spawned += 1;
        game->next_spawn_frame = game->frame + WAVE_SPAWN_INTERVAL;
        return;
    }

    if (game->wave_spawned >= enemies_in_wave &&
        enemies_active_count(game->enemies, MAX_ENEMIES) == 0) {
        game->wave += 1;
        game->wave_spawned = 0;
        game->next_spawn_frame = game->frame + WAVE_BREAK_FRAMES;
    }
}

void game_init(GameState *game)
{
    player_init(&game->player);
    projectiles_clear(game->player_shots, MAX_PLAYER_SHOTS);
    effect_clear(game->effects, MAX_EFFECTS);
    projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);
    enemies_clear(game->enemies, MAX_ENEMIES);

    game->frame = 0;
    game->wave = 1;
    game->wave_spawned = 0;
    game->next_spawn_frame = 0;
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
    update_wave_spawning(game);

    projectiles_update(game->player_shots,
                       MAX_PLAYER_SHOTS,
                       game->frame,
                       PLAYER_SHOT_MOVE_INTERVAL);
    projectiles_update(game->enemy_shots,
                       MAX_ENEMY_SHOTS,
                       game->frame,
                       enemy_shot_move_interval_for_wave(game->wave));
    enemies_update(game->enemies,
                   MAX_ENEMIES,
                   game->enemy_shots,
                   MAX_ENEMY_SHOTS,
                   game->frame,
                   enemy_move_interval_for_wave(game->wave));
    collisions_update(game);

    game->frame += 1;
    effect_update(game->effects, MAX_EFFECTS);
}
