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
}

void player_update(Player *player, int input_mask, Projectile player_shots[], int shot_count)
{
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

    if (player->position.x < 0) {
        player->position.x = 0;
    }
    if (player->position.x >= GAME_WIDTH) {
        player->position.x = GAME_WIDTH - 1;
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

    if ((input_mask & INPUT_FIRE) && player->shot_cooldown == 0) {
        Vec2i shot_position = {player->position.x, player->position.y - 1};
        Vec2i shot_velocity = {0, -1};
        projectiles_spawn(player_shots, shot_count, shot_position, shot_velocity);
        player->shot_cooldown = 3;
    }
}
