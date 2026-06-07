#include "game.h"

#include "collision.h"
#include "enemy.h"
#include "player.h"
#include "projectile.h"
#include "effect.h"
#include "highscore.h"
#include "input.h"
#include "powerup.h"

#include <ctype.h>
#include <string.h>

/************************************************************************
* Función: 
    spawn_x_for_wave
* Descripción: 
    Obtiene la cantidad de enemigos máxima que se pueden tener por ola en
        pantalla, esto para asegurar que la pantalla no se sature con 
        enemigos
* Entradas: 
    Puntero a instancia de objeto GameState (juego)
* Salidas: 
    Cantidad de enemigos a generar por ola
*************************************************************************/
static int spawn_x_for_wave(const GameState *game)
{
    //Calcula el ancho usable del área de juego para generar enemigos
    int usable_width = GAME_WIDTH - 6;

    /*Calcula la santidad de enemigos al sumar:
        Número de frame actual multiplicado por 7
        Número de ola generada multiplicado por 11
    Para después sumarle a 3, el residuo de dividir ese resultado entre el ancho usable calculado anteriormente
    */
    return 3 + ((game->frame * 7 + game->wave_spawned * 11) % usable_width);
}

/************************************************************************
* Función: 
    enemy_move_interval_for_wave
* Descripción: 
    Obtiene el intervalo de frames entre cada movimiento del enemigo, este
        irá en decremento conforme vaya avanzando el juego.
    Además, entre menor sea este intervalo, los enemigos se moverán más 
        y más rápido
* Entradas: 
    Número entero que indica número de ola actual
* Salidas: 
    Intervalo de movimiento (en frames) de los enemigos
*************************************************************************/
static int enemy_move_interval_for_wave(int wave)
{
    //Si la ola actual es menor o igual a la tercera
    if (wave <= 3) {
        //Retorna el intervalo base (Definido en config.h)
        return ENEMY_BASE_MOVE_INTERVAL;
    }
    //Si la ola es menor o igual a la octava 
    if (wave <= 8) {
        //Retorna el intervalo base decrementado una vez, es decir, los enemigos 
        //  podrán moverse más rápido
        return ENEMY_BASE_MOVE_INTERVAL - 1;
    }

    //Si el juego se encuentra más allá de la ola 8, retornará el intervalo base decrementado dos veces
    return ENEMY_BASE_MOVE_INTERVAL - 2;
}

/************************************************************************
* Función: 
    enemy_spawn_interval_for_rank
* Descripción: 
    Obtiene el intervalo de frames entre cada generación de enemigo este
        irá en decremento conforme vaya avanzando el juego.
    Además, entre menor sea este intervalo, los enemigos se generarán más 
        y más rápido
* Entradas: 
    Número entero que indica rango de jugador actual
* Salidas: 
    Intervalo de generación (en frames) de los enemigos
*************************************************************************/
static int enemy_spawn_interval_for_rank(int rank)
{
    //Calcula el speedup de generación el tomar el rango del jugador y dividirlo entre 3
    int speedup = rank / 3;

    //Si este speedup es menor a 5
    if (speedup > 5) {
        //Define el valor del speedup entre 3
        speedup = 5;
    }

    //Retornará el resultado de restarle el valor de speedup obtenido al intervalo de generación base (Definido en config.h)
    return WAVE_SPAWN_INTERVAL - speedup;
}

/************************************************************************
* Función: 
    rank_for_score
* Descripción: 
    Obtiene el rango o nivel actual de juego, de acuerdo a la puntuación
        actual del usuario
* Entradas: 
    Número entero que indica puntuación de jugador 
* Salidas: 
    Ragno o nivel actual del juego
*************************************************************************/
static int rank_for_score(int score)
{   
    //Este rango iniciará en 1, por tanto, se tomará la puntuación ingresada y se dividirá entre el intervalo de puntuación 
    //    necesaria para subir de nivel/rango, a este resultado se le sumará 1
    return 1 + (score / SCORE_RANK_INTERVAL);
}

/************************************************************************
* Función: 
    rank_for_score
* Descripción: 
    Obtiene la puntuación necesaria para poder llegar al próximo jefe
* Entradas: 
    Número entero que indica la cantidad de jefes dentro del juego
* Salidas: 
    Puntuación necesaria para poder llegar al próximo jefe
*************************************************************************/
static int next_boss_score_after_count(int boss_count)
{
    /*Suma:
        El intervalo de puntuación entre cada aparición de jefe
        La cantidad de jefes en el juego multiplicado por el crecimiento del intervalo anteriormente mencionado (Dato definido 
            en config.c)
    Retorna el resultado de esta suma*/
    return BOSS_SCORE_INTERVAL + boss_count * BOSS_SCORE_INTERVAL_GROWTH;
}

/************************************************************************
* Función: 
    boss_type_for_count
* Descripción: 
    Obtiene el tipo de jefe que sigue según la cantidad de jefes que hay 
        en el juego
* Entradas: 
    Número entero que indica la cantidad de jefes dentro del juego
* Salidas: 
    Siguiente tipo de jefe, si se desea agregar uno nuevo
*************************************************************************/
static EnemyType boss_type_for_count(int boss_count)
{
    //Toma el residuo de dividir entre cinco el resultado de incrementar la cantidad de enemigos una vez
    //Si este residuo es igual a cero 
    if ((boss_count + 1) % 5 == 0) {
        //Retorna que el siguiente jefe debe ser un jefe de nivel
        return ENEMY_STAGE_BOSS;
    }
    //De lo contrario, retorna que el siguiente jefe debe ser un mini jefe 
    return ENEMY_MINI_BOSS;
}

/************************************************************************
* Función: 
    enemy_shot_move_interval_for_wave
* Descripción: 
    Calcula el intervalo (en frames) entre disparos del enemigo según la
        ola en la que se encuentre el juego
* Entradas: 
    Número entero que indica la ola en la que se encuentra el juego
* Salidas: 
    intervalo (en frames) entre disparos del enemigo
*************************************************************************/
static int enemy_shot_move_interval_for_wave(int wave)
{   
    //Si la ola en la que se encuentra el juego es menor o igual a la octava
    if (wave <= 8) {
        //Retorna el valor de intervalo base (valor definido en config.h)
        return ENEMY_SHOT_BASE_MOVE_INTERVAL;
    }

    //De lo contrario, retorna 1, es decir, habrá un disparo por cada frame del juego
    return 1;
}

/************************************************************************
* Función: 
    abs_int
* Descripción: 
    Función matemática de valor absoluto
    Retorna el valor sin signo, es decir, siempre lo retornará positivo
* Entradas: 
    Número entero al que se le quiere calcular su valor absoluto
* Salidas: 
    Valor absoluto del número ingresado
*************************************************************************/
static int abs_int(int value)
{
    /*Evalúa si el valor es menor a cero
        Si el valor es menor a cero (Negativo) -> Retorna el negativo de este valor (Por ley de signos, - * - = +)
        Si el valor es igual o mayor a cero (Positivo) -> Retorna ese valor sin cambio alguno*/
    return value < 0 ? -value : value;
}

/************************************************************************
* Función: 
    score_for_enemy_type
* Descripción: 
    Busca cual es el puntaje correcto a otorgar al jugador dependiendo 
    del tipo de enemigo que este haya eliminado
* Entradas: 
    Tipo de enemigo eliminado EnemyType
* Salidas: 
    Puntaje correspondiente para cada tipo de enemigo
*************************************************************************/
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

/************************************************************************
* Función: 
    player_charge_position
* Descripción: 
    Calcula cual va a ser la coordenada desde donde debe salir el disparo
        cargado del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Vector con coordenadas (x,y) desde donde debe salir el disparo
*************************************************************************/
static Vec2i player_charge_position(const Player *player)
{   //Define un nuevo vector donde:
    //  Toma la coordenada x del jugador sin modificar
    //  Toma la coordenada y del jugador y la decrementa una vez, esto para hacer
    //      que el disparo cargado salga desde arriba del jugador
    Vec2i position = {player->position.x, player->position.y - 1};

    //Retorna el nuevo vector
    return position;
}

/************************************************************************
* Función: 
    projectile_inside_radius
* Descripción: 
    Verifica si el proyectil se encuentra dentro del rango especificado
* Entradas: 
    Vector con coordenadas (x,y) del projectil a verificar
    Vector con coordenadas (x,y) del centro del radio que se desea 
    verificar
    Número entero que indica el tamaño del radio a verificar, partiendo
        desde el centro de este
* Salidas: 
    1 -> El proyectil se encuentra dentro del rango especificado
    0 -> El proyectil se encuentra fuera del rango especificado
*************************************************************************/
static int projectile_inside_radius(Vec2i projectile, Vec2i center, int radius)
{
    /*Suma:
        Valor absoluto de la resta entre la coordenada x tanto del proyectil como del centro del radio
        Valor absoluto de la resta entre la coordenada y tanto del proyectil como del centro del radio
    El resultado de esta suma se compara con el radio especificado y retorna si el proyectil se encuentra dentro o fuera de este*/
    return abs_int(projectile.x - center.x) + abs_int(projectile.y - center.y) <= radius;
}

static int enemy_inside_radius(const Enemy *enemy, Vec2i center, int radius)
{
    int dx = 0;
    int dy = 0;

    if (center.x < enemy_left(enemy)) {
        dx = enemy_left(enemy) - center.x;
    } else if (center.x > enemy_right(enemy)) {
        dx = center.x - enemy_right(enemy);
    }

    if (center.y < enemy_top(enemy)) {
        dy = enemy_top(enemy) - center.y;
    } else if (center.y > enemy_bottom(enemy)) {
        dy = center.y - enemy_bottom(enemy);
    }

    return dx + dy <= radius;
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
    powerups_clear(game->powerups, MAX_POWERUPS);
    enemies_clear(game->enemies, MAX_ENEMIES);

    game->frame = 0;
    game->level = 1;
    game->boss_count = 0;
    game->next_boss_score = BOSS_SCORE_INTERVAL;
    game->next_extra_life_score = EXTRA_LIFE_SCORE_INTERVAL;
    game->wave_spawned = 0;
    game->next_spawn_frame = 0;
    game->phase = LEVEL_PHASE_NORMAL;
    game->status_message[0] = '\0';
    game->status_message_timer = 0;
    game->name_input[0] = '\0';
    game->name_length = 0;
    game->score_recorded = 0;
}

static void set_status_message(GameState *game, const char *message)
{
    strncpy(game->status_message, message, STATUS_MESSAGE_LENGTH - 1);
    game->status_message[STATUS_MESSAGE_LENGTH - 1] = '\0';
    game->status_message_timer = STATUS_MESSAGE_DURATION;
}

static void update_extra_life(GameState *game)
{
    while (game->player.score >= game->next_extra_life_score) {
        if (game->player.lives < PLAYER_MAX_LIVES) {
            game->player.lives += 1;
            set_status_message(game, "LIFE RECOVERED");
        }

        game->next_extra_life_score += EXTRA_LIFE_SCORE_INTERVAL;
    }
}

static void begin_name_entry(GameState *game)
{
    game->name_input[0] = '\0';
    game->name_length = 0;
    game->screen = GAME_SCREEN_NAME_ENTRY;
}

static void submit_high_score(GameState *game)
{
    HighScoreEntry entry;

    if (game->name_length == 0) {
        strncpy(game->name_input, "PLAYER", PLAYER_NAME_MAX_LENGTH);
        game->name_input[PLAYER_NAME_MAX_LENGTH] = '\0';
    }

    strncpy(entry.name, game->name_input, PLAYER_NAME_MAX_LENGTH);
    entry.name[PLAYER_NAME_MAX_LENGTH] = '\0';
    entry.score = game->player.score;
    entry.rank = game->level;
    entry.bosses = game->boss_count;

    highscores_add(game->high_scores, MAX_HIGH_SCORES, entry);
    highscores_save(game->high_scores, MAX_HIGH_SCORES, HIGHSCORE_FILE_NAME);

    game->score_recorded = 1;
    game->screen = GAME_SCREEN_GAME_OVER;
}

static void update_name_entry(GameState *game, int input_mask)
{
    int character = input_last_character();

    if (input_mask & INPUT_START) {
        submit_high_score(game);
        return;
    }

    if ((input_mask & INPUT_BACKSPACE) && game->name_length > 0) {
        game->name_length -= 1;
        game->name_input[game->name_length] = '\0';
        return;
    }

    if (character >= 0 &&
        character <= 255 &&
        isalnum((unsigned char)character) &&
        game->name_length < PLAYER_NAME_MAX_LENGTH) {
        game->name_input[game->name_length] = (char)toupper((unsigned char)character);
        game->name_length += 1;
        game->name_input[game->name_length] = '\0';
    }
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
        game->next_spawn_frame = game->frame + enemy_spawn_interval_for_rank(game->level);
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
    highscores_load(game->high_scores, MAX_HIGH_SCORES, HIGHSCORE_FILE_NAME);
    game->running = 1;
    game->screen = GAME_SCREEN_MENU;
    game->previous_screen = GAME_SCREEN_MENU;
}

void game_update(GameState *game, int input_mask)
{
    if (game->screen == GAME_SCREEN_NAME_ENTRY) {
        update_name_entry(game, input_mask);
        return;
    }

    if (input_mask & INPUT_QUIT) {
        game->running = 0;
        return;
    }

    if (game->screen == GAME_SCREEN_HELP) {
        if (input_mask & (INPUT_HELP | INPUT_START | INPUT_PAUSE)) {
            game->screen = game->previous_screen;
        }
        return;
    }

    if (input_mask & INPUT_HELP) {
        game->previous_screen = game->screen;
        game->screen = GAME_SCREEN_HELP;
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

    if (game->screen == GAME_SCREEN_PAUSED) {
        if (input_mask & INPUT_PAUSE) {
            game->screen = GAME_SCREEN_PLAYING;
        }
        return;
    }

    if (input_mask & INPUT_PAUSE) {
        game->screen = GAME_SCREEN_PAUSED;
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
    powerups_update(game->powerups,
                    MAX_POWERUPS,
                    game->frame,
                    POWERUP_MOVE_INTERVAL);
    enemies_update(game->enemies,
                   MAX_ENEMIES,
                   game->enemy_shots,
                   MAX_ENEMY_SHOTS,
                   game->frame,
                   enemy_move_interval_for_wave(game->level));
    update_charge_shield(game);
    collisions_update(game);
    update_extra_life(game);
    game->level = rank_for_score(game->player.score);
    update_level_progression(game);

    if (game->status_message_timer > 0) {
        game->status_message_timer -= 1;
    }

    if (game->player.lives <= 0) {
        if (!game->score_recorded) {
            begin_name_entry(game);
        } else {
            game->screen = GAME_SCREEN_GAME_OVER;
        }
    }

    game->frame += 1;
    effect_update(game->effects, MAX_EFFECTS);
}
