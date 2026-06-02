#ifndef RECCA_GAME_H
#define RECCA_GAME_H

#include "config.h"
#include "types.h"

typedef struct {
    Player player;
    Projectile player_shots[MAX_PLAYER_SHOTS];
    Projectile enemy_shots[MAX_ENEMY_SHOTS];
    PowerUp powerups[MAX_POWERUPS];
    Enemy enemies[MAX_ENEMIES];
    Effect effects[MAX_EFFECTS];
    int frame;
    int level;
    int boss_count;
    int next_boss_score;
    int wave_spawned;
    int next_spawn_frame;
    LevelPhase phase;
    int running;
    GameScreen screen;
} GameState;

void game_init(GameState *game);
void game_update(GameState *game, int input_mask);

#endif
