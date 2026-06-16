#ifndef RECCA_TYPES_H
#define RECCA_TYPES_H

#include "config.h"
#include <time.h>

typedef struct {
    int x;
    int y;
} Vec2i;

typedef struct {
    Vec2i position;
    Vec2i velocity;
    int active;
} Projectile;

typedef struct {
    char name[PLAYER_NAME_MAX_LENGTH + 1];
    int score;
    int rank;
    int bosses;
} HighScoreEntry;

typedef enum {
    WEAPON_FRONT,
    WEAPON_SPREAD,
    WEAPON_LASER,
    WEAPON_DOUBLE,
    WEAPON_SIDE
} WeaponType;

typedef enum {
    POWERUP_WEAPON,
    POWERUP_DRONE
} PowerUpType;

typedef struct {
    Vec2i position;
    int active;
    PowerUpType type;
    WeaponType weapon;
} PowerUp;

typedef struct {
    Vec2i position;
    int active;
    int timer;
} Effect;

typedef enum {
    ENEMY_STRAIGHT,
    ENEMY_DIAGONAL,
    ENEMY_ZIGZAG,
    ENEMY_FAST,
    ENEMY_SWEEP,
    ENEMY_DIVE,
    ENEMY_MINI_BOSS,
    ENEMY_STAGE_BOSS
} EnemyType;

typedef struct {
    Vec2i position;
    Vec2i velocity;
    int active;
    int health;
    int fire_cooldown;
    int age;
    EnemyType type;
} Enemy;

typedef struct {
    Vec2i position;
    int lives;
    int score;
    int shot_cooldown;
    int invulnerable_timer;
    int charge_frames;
    int charge_bomb_ready;
    int drone_count;
    int drone_timer;
    int weapon_timer;
    WeaponType weapon;
} Player;

typedef struct {
    struct timespec start;    // Guarda el timestamp nativo exacto de inicio
    double last_time;         // Duración de la última llamada individual
    double total_time;        // Acumulador de tiempo (para promedios)
    unsigned long long count; // Cuántas veces se ha ejecutado el módulo
} Timer;

typedef enum {
    INPUT_NONE = 0,
    INPUT_LEFT = 1 << 0,
    INPUT_RIGHT = 1 << 1,
    INPUT_UP = 1 << 2,
    INPUT_DOWN = 1 << 3,
    INPUT_FIRE = 1 << 4,
    INPUT_START = 1 << 5,
    INPUT_RESTART = 1 << 6,
    INPUT_QUIT = 1 << 7,
    INPUT_PAUSE = 1 << 8,
    INPUT_HELP = 1 << 9,
    INPUT_BACKSPACE = 1 << 10
} InputMask;

typedef enum {
    GAME_SCREEN_MENU,
    GAME_SCREEN_PLAYING,
    GAME_SCREEN_PAUSED,
    GAME_SCREEN_HELP,
    GAME_SCREEN_NAME_ENTRY,
    GAME_SCREEN_GAME_OVER
} GameScreen;

typedef enum {
    LEVEL_PHASE_NORMAL,
    LEVEL_PHASE_BOSS
} LevelPhase;

#endif
