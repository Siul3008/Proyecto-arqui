#include "powerup.h"

#include "config.h"

static int inside_board(Vec2i position)
{
    return position.x >= 0 && position.x < GAME_WIDTH &&
           position.y >= 0 && position.y < GAME_HEIGHT;
}

void powerups_clear(PowerUp powerups[], int count)
{
    for (int i = 0; i < count; ++i) {
        powerups[i].active = 0;
        powerups[i].position.x = 0;
        powerups[i].position.y = 0;
        powerups[i].weapon = WEAPON_FRONT;
    }
}

void powerups_spawn(PowerUp powerups[], int count, Vec2i position, WeaponType weapon)
{
    for (int i = 0; i < count; ++i) {
        if (!powerups[i].active) {
            powerups[i].active = 1;
            powerups[i].position = position;
            powerups[i].weapon = weapon;
            return;
        }
    }
}

void powerups_update(PowerUp powerups[], int count, int frame, int move_interval)
{
    if (move_interval <= 0 || frame % move_interval != 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        if (!powerups[i].active) {
            continue;
        }

        powerups[i].position.y += 1;

        if (!inside_board(powerups[i].position)) {
            powerups[i].active = 0;
        }
    }
}

WeaponType powerup_weapon_for_seed(int seed)
{
    switch (seed % 3) {
    case 0:
        return WEAPON_FRONT;
    case 1:
        return WEAPON_SPREAD;
    default:
        return WEAPON_LASER;
    }
}
