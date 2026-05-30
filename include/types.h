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

typedef struct {
    Vec2i position;
    int active;
    int timer;
} Effect;

typedef enum {
    ENEMY_STRAIGHT,
    ENEMY_DIAGONAL,
    ENEMY_ZIGZAG,
    ENEMY_FAST
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
} Player;

typedef enum {
    INPUT_NONE = 0,
    INPUT_LEFT = 1 << 0,
    INPUT_RIGHT = 1 << 1,
    INPUT_UP = 1 << 2,
    INPUT_DOWN = 1 << 3,
    INPUT_FIRE = 1 << 4,
    INPUT_QUIT = 1 << 5
} InputMask;

#endif
