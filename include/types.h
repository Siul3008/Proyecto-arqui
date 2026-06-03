#ifndef RECCA_TYPES_H
#define RECCA_TYPES_H

typedef struct {
    int x;
    int y;
} Vec2i;

typedef struct {
    Vec2i position;
    Vec2i velocity;
    int active;
} Projectile;

typedef enum {
    WEAPON_FRONT,
    WEAPON_SPREAD,
    WEAPON_LASER
} WeaponType;

typedef struct {
    Vec2i position;
    int active;
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
    WeaponType weapon;
} Player;

typedef enum {
    INPUT_NONE = 0,
    INPUT_LEFT = 1 << 0,
    INPUT_RIGHT = 1 << 1,
    INPUT_UP = 1 << 2,
    INPUT_DOWN = 1 << 3,
    INPUT_FIRE = 1 << 4,
    INPUT_START = 1 << 5,
    INPUT_RESTART = 1 << 6,
    INPUT_QUIT = 1 << 7
} InputMask;

typedef enum {
    GAME_SCREEN_MENU,
    GAME_SCREEN_PLAYING,
    GAME_SCREEN_GAME_OVER
} GameScreen;

typedef enum {
    LEVEL_PHASE_NORMAL,
    LEVEL_PHASE_BOSS
} LevelPhase;

#endif
