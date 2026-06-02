#include "projectile.h"

#include "config.h"

static int inside_board(Vec2i position)
{
    return position.x >= 0 && position.x < GAME_WIDTH &&
           position.y >= 0 && position.y < GAME_HEIGHT;
}

void projectiles_clear(Projectile projectiles[], int count)
{
    for (int i = 0; i < count; ++i) {
        projectiles[i].active = 0;
        projectiles[i].position.x = 0;
        projectiles[i].position.y = 0;
        projectiles[i].velocity.x = 0;
        projectiles[i].velocity.y = 0;
    }
}

void projectiles_spawn(Projectile projectiles[], int count, Vec2i position, Vec2i velocity)
{
    for (int i = 0; i < count; ++i) {
        if (!projectiles[i].active) {
            projectiles[i].active = 1;
            projectiles[i].position = position;
            projectiles[i].velocity = velocity;
            return;
        }
    }
}

void projectiles_update(Projectile projectiles[], int count, int frame, int move_interval)
{
    if (move_interval <= 0 || frame % move_interval != 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        if (!projectiles[i].active) {
            continue;
        }

        projectiles[i].position.x += projectiles[i].velocity.x;
        projectiles[i].position.y += projectiles[i].velocity.y;

        if (!inside_board(projectiles[i].position)) {
            projectiles[i].active = 0;
        }
    }
}
