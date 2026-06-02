#include "player.h"

#include "config.h"
#include "projectile.h"

void player_init(Player *player)
{
    player->position.x = PLAYER_START_X;
    player->position.y = PLAYER_START_Y;
    player->lives = PLAYER_START_LIVES;
    player->score = 0;
    player->shot_cooldown = 0;
    player->invulnerable_timer = 0;
    player->charge_frames = 0;
    player->charge_bomb_ready = 0;
    player->weapon = WEAPON_FRONT;
}

static void fire_front_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i shot_position = {player->position.x, player->position.y - 1};
    Vec2i shot_velocity = {0, -1};
    projectiles_spawn(player_shots, shot_count, shot_position, shot_velocity);
}

static void fire_spread_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i center = {player->position.x, player->position.y - 1};
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){-1, -1});
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){1, -1});
}

static void fire_laser_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i first = {player->position.x, player->position.y - 1};
    Vec2i second = {player->position.x, player->position.y - 2};
    projectiles_spawn(player_shots, shot_count, first, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, second, (Vec2i){0, -1});
}

static void fire_current_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    switch (player->weapon) {
    case WEAPON_FRONT:
        fire_front_weapon(player, player_shots, shot_count);
        break;
    case WEAPON_SPREAD:
        fire_spread_weapon(player, player_shots, shot_count);
        break;
    case WEAPON_LASER:
        fire_laser_weapon(player, player_shots, shot_count);
        break;
    }
}

void player_update(Player *player, int input_mask, Projectile player_shots[], int shot_count)
{
    player->charge_bomb_ready = 0;

    if (player->invulnerable_timer > 0) {
        player->invulnerable_timer -= 1;
    }

    if (input_mask & INPUT_LEFT) {
        player->position.x -= 1;
    }
    if (input_mask & INPUT_RIGHT) {
        player->position.x += 1;
    }
    if (input_mask & INPUT_UP) {
        player->position.y -= 1;
    }
    if (input_mask & INPUT_DOWN) {
        player->position.y += 1;
    }

    if (player->position.x < 1) {
        player->position.x = 1;
    }
    if (player->position.x >= GAME_WIDTH - 1) {
        player->position.x = GAME_WIDTH - 2;
    }
    if (player->position.y < 0) {
        player->position.y = 0;
    }
    if (player->position.y >= GAME_HEIGHT) {
        player->position.y = GAME_HEIGHT - 1;
    }

    if (player->shot_cooldown > 0) {
        player->shot_cooldown -= 1;
    }

    if (!(input_mask & INPUT_FIRE)) {
        if (player->charge_frames < PLAYER_CHARGE_MAX) {
            player->charge_frames += 1;
        }
        return;
    }

    if (player->charge_frames >= PLAYER_CHARGE_RELEASE_MIN) {
        player->charge_bomb_ready = 1;
        player->charge_frames = 0;
        player->shot_cooldown = PLAYER_SHOT_COOLDOWN;
        return;
    }

    player->charge_frames = 0;

    if (player->shot_cooldown == 0) {
        fire_current_weapon(player, player_shots, shot_count);
        player->shot_cooldown = PLAYER_SHOT_COOLDOWN;
    }
}
