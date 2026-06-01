#include "game.h"

#include "collision.h"
#include "enemy.h"
#include "player.h"
#include "projectile.h"
#include "effect.h"

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

static int rank_for_score(int score)
{
    return 1 + (score / SCORE_RANK_INTERVAL);
}

static int next_boss_score_after_count(int boss_count)
{
    return BOSS_SCORE_INTERVAL + boss_count * BOSS_SCORE_INTERVAL_GROWTH;
}

static EnemyType boss_type_for_count(int boss_count)
{
    if ((boss_count + 1) % 5 == 0) {
        return ENEMY_STAGE_BOSS;
    }

    return ENEMY_MINI_BOSS;
}

static int enemy_shot_move_interval_for_wave(int wave)
{
    if (wave <= 2) {
        return ENEMY_SHOT_BASE_MOVE_INTERVAL;
    }

    return 1;
}

static int abs_int(int value)
{
    return value < 0 ? -value : value;
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

static Vec2i player_charge_position(const Player *player)
{
    Vec2i position = {player->position.x, player->position.y - 1};
    return position;
}

static int projectile_inside_radius(Vec2i projectile, Vec2i center, int radius)
{
    return abs_int(projectile.x - center.x) + abs_int(projectile.y - center.y) <= radius;
}

static int enemy_inside_radius(const Enemy *enemy, Vec2i center, int radius)
{
    int half_width = enemy_hitbox_half_width(enemy->type);
    int left = enemy->position.x - half_width;
    int right = enemy->position.x + half_width;
    int dx = 0;

    if (center.x < left) {
        dx = left - center.x;
    } else if (center.x > right) {
        dx = center.x - right;
    }

    return dx + abs_int(enemy->position.y - center.y) <= radius;
}

static void spawn_charge_effects(GameState *game, Vec2i center)
{
    effect_spawn(game->effects, MAX_EFFECTS, center, EXPLOSION_DURATION);
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x - 1, center.y}, EXPLOSION_DURATION);
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x + 1, center.y}, EXPLOSION_DURATION);
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x, center.y - 1}, EXPLOSION_DURATION);
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x, center.y + 1}, EXPLOSION_DURATION);
}

static void update_charge_shield(GameState *game)
{
    if (game->player.charge_frames < PLAYER_CHARGE_SHIELD_MIN) {
        return;
    }

    Vec2i center = player_charge_position(&game->player);

    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        Projectile *shot = &game->enemy_shots[i];

        if (shot->active && projectile_inside_radius(shot->position, center, CHARGE_SHIELD_RADIUS)) {
            effect_spawn(game->effects, MAX_EFFECTS, shot->position, EXPLOSION_DURATION);
            shot->active = 0;
        }
    }
}

static void apply_charge_bomb(GameState *game)
{
    if (!game->player.charge_bomb_ready) {
        return;
    }

    Vec2i center = player_charge_position(&game->player);
    spawn_charge_effects(game, center);

    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        Projectile *shot = &game->enemy_shots[i];

        if (shot->active && projectile_inside_radius(shot->position, center, CHARGE_BOMB_RADIUS)) {
            shot->active = 0;
        }
    }

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy *enemy = &game->enemies[i];

        if (!enemy->active || !enemy_inside_radius(enemy, center, CHARGE_BOMB_RADIUS)) {
            continue;
        }

        enemy->health -= CHARGE_BOMB_DAMAGE;
        if (enemy->health <= 0) {
            effect_spawn(game->effects, MAX_EFFECTS, enemy->position, EXPLOSION_DURATION);
            enemy->active = 0;
            game->player.score += score_for_enemy_type(enemy->type);
        }
    }
}

static void reset_run(GameState *game)
{
    player_init(&game->player);
    projectiles_clear(game->player_shots, MAX_PLAYER_SHOTS);
    effect_clear(game->effects, MAX_EFFECTS);
    projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);
    enemies_clear(game->enemies, MAX_ENEMIES);

    game->frame = 0;
    game->level = 1;
    game->boss_count = 0;
    game->next_boss_score = BOSS_SCORE_INTERVAL;
    game->wave_spawned = 0;
    game->next_spawn_frame = 0;
    game->phase = LEVEL_PHASE_NORMAL;
}

static void update_wave_spawning(GameState *game)
{
    if (game->phase != LEVEL_PHASE_NORMAL) {
        return;
    }

    if (game->player.score >= game->next_boss_score) {
        enemies_clear(game->enemies, MAX_ENEMIES);
        projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);
        enemies_spawn_boss(game->enemies, MAX_ENEMIES, boss_type_for_count(game->boss_count));
        game->phase = LEVEL_PHASE_BOSS;
        return;
    }

    if (game->frame >= game->next_spawn_frame) {
        EnemyType type = enemy_type_for_wave(game->level, game->wave_spawned);
        enemies_spawn(game->enemies, MAX_ENEMIES, spawn_x_for_wave(game), type);
        game->wave_spawned += 1;
        game->next_spawn_frame = game->frame + WAVE_SPAWN_INTERVAL - (game->level > 5 ? 5 : game->level);
    }
}

static void update_level_progression(GameState *game)
{
    if (game->phase == LEVEL_PHASE_BOSS &&
        !enemies_has_boss(game->enemies, MAX_ENEMIES)) {
        game->boss_count += 1;
        game->next_boss_score += next_boss_score_after_count(game->boss_count);
        game->wave_spawned = 0;
        game->next_spawn_frame = game->frame + WAVE_BREAK_FRAMES;
        game->phase = LEVEL_PHASE_NORMAL;
        projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);
    }
}

void game_init(GameState *game)
{
    reset_run(game);
    game->running = 1;
    game->screen = GAME_SCREEN_MENU;
}

void game_update(GameState *game, int input_mask)
{
    if (input_mask & INPUT_QUIT) {
        game->running = 0;
        return;
    }

    if (game->screen == GAME_SCREEN_MENU) {
        if (input_mask & INPUT_START) {
            reset_run(game);
            game->screen = GAME_SCREEN_PLAYING;
        }
        return;
    }

    if (game->screen == GAME_SCREEN_GAME_OVER) {
        if (input_mask & INPUT_RESTART) {
            reset_run(game);
            game->screen = GAME_SCREEN_PLAYING;
        }
        return;
    }

    player_update(&game->player, input_mask, game->player_shots, MAX_PLAYER_SHOTS);
    apply_charge_bomb(game);
    update_wave_spawning(game);

    projectiles_update(game->player_shots,
                       MAX_PLAYER_SHOTS,
                       game->frame,
                       PLAYER_SHOT_MOVE_INTERVAL);
    projectiles_update(game->enemy_shots,
                       MAX_ENEMY_SHOTS,
                       game->frame,
                       enemy_shot_move_interval_for_wave(game->level));
    enemies_update(game->enemies,
                   MAX_ENEMIES,
                   game->enemy_shots,
                   MAX_ENEMY_SHOTS,
                   game->frame,
                   enemy_move_interval_for_wave(game->level));
    update_charge_shield(game);
    collisions_update(game);
    game->level = rank_for_score(game->player.score);
    update_level_progression(game);

    if (game->player.lives <= 0) {
        game->screen = GAME_SCREEN_GAME_OVER;
    }

    game->frame += 1;
    effect_update(game->effects, MAX_EFFECTS);
}
