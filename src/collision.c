#include "collision.h"

#include "config.h"
#include "effect.h"
#include "enemy.h"
#include "powerup.h"

#include <string.h>

/********************************************************************
* Función: 
    positions_overlap
* Descripción: 
    Verifica si las coordenadas de dos elementos son iguales
* Entradas: 
    Vectores de posición (x,y) de elementos "a" y "b"
* Salidas: 
    1 => Hay "overlap" entre elementos (mismas coordenadas)
    0 => No hay "overlap" entre elementos (distintas coordenadas)
********************************************************************/
static int positions_overlap(Vec2i a, Vec2i b)
{    
    return a.x == b.x && a.y == b.y;
}

/************************************************************************
* Función: 
    ranges_overlap
* Descripción: 
    Verifica si hay un "overlap" entre rangos o "hitboxes" de elementos
* Entradas: 
    Posiciones de cada costado (derecha e izquierda o arriba y abajo, 
    según lo requerido en el ingreso de valores) del "hitbox" de 
    elementos a y b
* Salidas: 
    1 => Hay "overlap" entre "hitboxes"
    0 => No hay "overlap" entre "hitboxes"
*************************************************************************/
static int ranges_overlap(int left_a, int right_a, int left_b, int right_b)
{
    return left_a <= right_b && left_b <= right_a;
}

/************************************************************************
* Función: 
    player_left
* Descripción: 
    Obtiene la coordenada x del lado izquierdo de la hitbox del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Coordenada x del lado izquierdo de la hitbox del jugador
*************************************************************************/
static int player_left(const Player *player)
{
    return player->position.x;
}

/************************************************************************
* Función: 
    player_right
* Descripción: 
    Obtiene la coordenada x del lado derecho de la hitbox del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Coordenada x del lado derecho de la hitbox del jugador
*************************************************************************/
static int player_right(const Player *player)
{
    return player->position.x + 2;
}

/************************************************************************
* Función: 
    player_top
* Descripción: 
    Obtiene la coordenada y del lado superior de la hitbox del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Coordenada y del lado superior de la hitbox del jugador
*************************************************************************/
static int player_top(const Player *player)
{
    return player->position.y;
}

/************************************************************************
* Función: 
    player_bottom
* Descripción: 
    Obtiene la coordenada y del lado inferior de la hitbox del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Coordenada y del lado inferior de la hitbox del jugador
*************************************************************************/
static int player_bottom(const Player *player)
{
    return player->position.y;
}

/************************************************************************
* Función: 
    projectile_hits_player
* Descripción: 
    Verifica si el proyectil ha impactado al jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Vector (x,y) con coordenadas de proyectil a verificar
* Salidas: 
    1 => El proyectil ha impactado contra el jugador
    0 => El proyectil no ha impactado contra el jugador
*************************************************************************/
static int projectile_hits_player(Vec2i shot_position, const Player *player)
{
    return shot_position.y >= player_top(player) &&
           shot_position.y <= player_bottom(player) &&
           shot_position.x >= player_left(player) &&
           shot_position.x <= player_right(player);
}

/************************************************************************
* Función: 
    projectile_hits_enemy
* Descripción: 
    Verifica si el proyectil ha impactado a a la nave enemiga
* Entradas: 
    Puntero a instancia de objeto Enemy (enemigo)
    Vector (x,y) con coordenadas de proyectil a verificar
* Salidas: 
    1 => El proyectil ha impactado contra el enemigo
    0 => El proyectil no ha impactado contra el enemigo
*************************************************************************/
static int projectile_hits_enemy(Vec2i shot_position, const Enemy *enemy)
{
    return shot_position.y >= enemy_top(enemy) &&
           shot_position.y <= enemy_bottom(enemy) &&
           shot_position.x >= enemy_left(enemy) &&
           shot_position.x <= enemy_right(enemy);
}

/************************************************************************
* Función: 
    player_hits_enemy
* Descripción: 
    Verifica si el jugador ha impactado contra la nave enemiga
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    1 => El jugador ha impactado contra el enemigo
    0 => El jugador no ha impactado contra el enemigo
*************************************************************************/
static int player_hits_enemy(const Player *player, const Enemy *enemy)
{
    return ranges_overlap(player_top(player), player_bottom(player), enemy_top(enemy), enemy_bottom(enemy)) &&
           ranges_overlap(player_left(player), player_right(player), enemy_left(enemy), enemy_right(enemy));
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
    enemy_should_drop_powerup
* Descripción: 
    Verifica si el enemigo eliminado debería "soltar" un power-up que el
    jugador puede usar si atrapa el mismo
* Entradas: 
    Puntero a instancia de objeto GameState (juego)
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    1 => El enemigo debe "soltar" un power-up
    0 => El enemigo no debe "soltar" un power-up
*************************************************************************/
static int enemy_should_drop_powerup(const GameState *game, const Enemy *enemy)
{
    //Si el enemigo es algún jefe
    if (enemy->type == ENEMY_MINI_BOSS || enemy->type == ENEMY_STAGE_BOSS) {
        return 1;
    }

    /*Si el enemigo no es un jefe, determina si este debe soltar un power up al tomar el residuo de dividir entre el divisor
    de powerup (definido en config.h), el resultado de sumar:
        El número de frame actual
        La coordenada x del enemigo
        El puntaje hasta el momento del jugador

    Si el resultado de estas operaciones es 0, retornará un 1, de lo contrario, retornará un 0
    */
    return ((game->frame + enemy->position.x + game->player.score) % POWERUP_DROP_DIVISOR) == 0;
}

/************************************************************************
* Función: 
   drop_powerup_from_enemy
* Descripción: 
    Si el enemigo debe "soltar" un power-up, crea una semilla para
    la generación del mismo, además, dependiendo del tipo de enemigo que
    sea, suelta un tipo diferente de power-up en comparación a un enemigo 
    común, este usará la semilla generada anteriormente
* Entradas: 
    Puntero a instancia de objeto GameState (juego)
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Se debe soltar power-up => Power-up correspondiente a tipo de enemigo 
    No se debe soltar power-up => Nada
*************************************************************************/
static void drop_powerup_from_enemy(GameState *game, const Enemy *enemy)
{
    //Verifica si el enemigo debe soltar un power-up
    if (!enemy_should_drop_powerup(game, enemy)) {
        return;
    }

    //Genera semilla usada para la generación de power-up
    //  Será una suma del # de frame en el que se encuentra el juego, el puntaje actual del jugador,
    //      así como las coordenadas (x,y) del enemigo que acaba de ser eliminado
    int seed = game->frame + game->player.score + enemy->position.x + enemy->position.y;

    //Verifica si el enemigo eliminado ha sido un jefe
    if (enemy->type == ENEMY_STAGE_BOSS) {
        //Si lo es, genera tanto un dron como un arma, en este caso, esta última será un láser
        powerups_spawn_drone(game->powerups, MAX_POWERUPS, enemy->position);
        powerups_spawn_weapon(game->powerups,
                              MAX_POWERUPS,
                              (Vec2i){enemy->position.x, enemy->position.y + 1},
                              seed % 2 == 0 ? WEAPON_LASER : WEAPON_SIDE);
        return;
    }

    //Verifica si el enemigo eliminado ha sido un mini jefe
    if (enemy->type == ENEMY_MINI_BOSS) {
        WeaponType rewards[] = {
            WEAPON_SPREAD,
            WEAPON_DOUBLE,
            WEAPON_LASER,
            WEAPON_SIDE
        };

        //Si lo es, verifica si la semilla es divisible entre 2, de serlo, soltará un arma tipo spread,
        //  de lo contrario, generará un láser
        powerups_spawn_weapon(game->powerups,
                              MAX_POWERUPS,
                              enemy->position,
                              rewards[seed % 4]);
        return;
    }

    //Si no es ni jefe, ni mini jefe, es un enemigo común

    //Verifica si la semilla es divisible entre 12
    if (seed % 12 == 0) {
        //Si lo es, genera un dron
        powerups_spawn_drone(game->powerups, MAX_POWERUPS, enemy->position);
    } else {
        //De lo contrario, soltará un arma usando la semilla como referencia
        powerups_spawn_weapon(game->powerups,
                              MAX_POWERUPS,
                              enemy->position,
                              powerup_weapon_for_seed(seed));
    }
}

/************************************************************************
* Función: 
    damage_player
* Descripción: 
    Si el jugador no se encuentra en estado de invencibilidad, se le resta
    una vida
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Ninguna
*************************************************************************/
static void damage_player(Player *player)
{
    if (player->invulnerable_timer > 0) {
        return;
    }

    player->lives -= 1;
    player->invulnerable_timer = PLAYER_INVULNERABLE_FRAMES;
}

/************************************************************************
* Función: 
    set_status_message
* Descripción: 
    Actualiza el mensaje de estado del juego
* Entradas: 
    Puntero a instancia de objeto GameState (juego)
    Puntero a secuencia constante de caracteres
* Salidas: 
    Ninguna
*************************************************************************/
static void set_status_message(GameState *game, const char *message)
{
    strncpy(game->status_message, message, STATUS_MESSAGE_LENGTH - 1);  //Copia la cadena ingresada al buffer destinado para presentar el mensaje
    game->status_message[STATUS_MESSAGE_LENGTH - 1] = '\0'; //Agrega un caracter nulo al final del buffer donde se presentará el mensaje en pantalla
    game->status_message_timer = STATUS_MESSAGE_DURATION; //Establece la duracion (en frames) del mensaje en pantalla

}

/************************************************************************
 * Función: 
    capped_timer_add    
* Descripción:
    Suma un valor a un temporizador, pero si el resultado excede un valor 
    máximo definido en config.h, se establece el temporizador en dicho 
    valor máximo
 * Entradas:
    Temporizador actual (en frames)
    Valor a sumar al temporizador (en frames)
* Salidas:
    Valor actualizado del temporizador (en frames)
*************************************************************************/
static int capped_timer_add(int current_timer, int added_timer)
{
    if (current_timer >= POWERUP_TIMER_MAX_FRAMES - added_timer) {
        return POWERUP_TIMER_MAX_FRAMES;                       
    }

    return current_timer + added_timer;
}

/************************************************************************
* Función: 
    weapon_duration_for_type
* Descripción: 
    Indica cual debe ser el tiempo de vida (en frames) de un arma
        según su tipo
* Entradas: 
    Tipo de arma WeaponType
* Salidas: 
    Tiempo de vida (en frames) del arma
*************************************************************************/
static int weapon_duration_for_type(WeaponType weapon)
{
    //Todos los valores que la función devolverá, han sido previamente definidos en config.h

    switch (weapon) {
    //Si el arma es un laser
    case WEAPON_LASER:
        return PLAYER_WEAPON_SHORT_DURATION_FRAMES;
    case WEAPON_SIDE:
        return PLAYER_WEAPON_MEDIUM_DURATION_FRAMES;
    //Si es un arma doble o esparcida
    case WEAPON_DOUBLE:
    case WEAPON_SPREAD:
        return PLAYER_WEAPON_LONG_DURATION_FRAMES;
    //Si es un arma tipo FRONT
    case WEAPON_FRONT:
        return PLAYER_WEAPON_DURATION_FRAMES;
    }

    //Si es el arlma normal
    return PLAYER_WEAPON_DURATION_FRAMES;
}

/************************************************************************
* Función: 
    weapon_status_message
* Descripción: 
    Obtiene el mensaje que debe ser desplegado dependiendo del tipo de 
        arma
* Entradas: 
    Tipo de arma WeaponType
* Salidas: 
    Cadena con mensaje a desplegar según tipo de arma
*************************************************************************/
static const char *weapon_status_message(WeaponType weapon)
{
    switch (weapon) {
    //Si el arma es tipo FRONT
    case WEAPON_FRONT:
        return "FRONT READY";
    //Si el arma es tipo SPREAD
    case WEAPON_SPREAD:
        return "SPREAD READY";
    //Si el arma es tipo LASER
    case WEAPON_LASER:
        return "LASER READY";
    //Si el arma es tipo DOUBLE
    case WEAPON_DOUBLE:
        return "DOUBLE READY";
    //Si el arma es tipo SIDE
    case WEAPON_SIDE:
        return "SIDE READY";
    }

    //Si no es ningun tipo de los anteriores, retorna el mensaje correspondiente al arma común
    return "WEAPON READY";
}

/************************************************************************
* Función: 
    collisions_update
* Descripción: 
    Actualiza el estado de todas las colisiones que pueden estar 
    ocurriendo en el juego
* Entradas: 
    Puntero a instancia de objeto GameState (juego)
* Salidas: 
    Ninguna
*************************************************************************/
void collisions_update(GameState *game)
{
    //Mientras i sea menor a la cantidad máxima de disparos permitidos al jugador al mismo tiempo
    for (int i = 0; i < MAX_PLAYER_SHOTS; ++i) {
        Projectile *shot = &game->player_shots[i];
        
        //Toma un disparo de la lista de disparos del jugador y verifica que este se encuentre activo

        if (!shot->active) {
            continue;
        }

        //Mientras j sea menor a la cantidad máxima de enemigos activos al mismo tiempo
        for (int j = 0; j < MAX_ENEMIES; ++j) {
            Enemy *enemy = &game->enemies[j];

            //Toma un enemigo de la lista de enemigos y verifica que este se encuentre activo

            if (!enemy->active) {
                continue;
            }

            //Si el disparo del jugador ha impactado contra un enemigo
            if (projectile_hits_enemy(shot->position, enemy)) {
                shot->active = 0; //Desactiva el disparo del jugador
                enemy->health -= 1; //Registra el daño causado al enemigo

                //Si la vida del enemigo es menor o igual a cero
                if (enemy->health <= 0) {
                    effect_spawn(game->effects, MAX_EFFECTS, enemy->position, EXPLOSION_DURATION); //Genera un efecto de explosión en la posición del enemigo
                    drop_powerup_from_enemy(game, enemy); //De ser posible, suelta un power-up al jugador
                    enemy->active = 0;  //Desactiva el enemigo
                    game->player.score += score_for_enemy_type(enemy->type); /*Le otorga al jugador la cantidad de puntos correspondientes según el tipo de enemigo y
                                                                                lo suma al puntaje final*/
                }
                break;
            }
        }
    }

    //Mientras i sea menor a la cantidad máxima de disparos permitidos a los enemigos al mismo tiempo
    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        Projectile *shot = &game->enemy_shots[i];

        //Toma un disparo de la lista de disparos de los enemigos y verifica que este se encuentre activo

        if (!shot->active) {
            continue;
        }

        //Si el disparo del enemigo ha impactado contra el jugador
        if (projectile_hits_player(shot->position, &game->player)) {
            shot->active = 0; //Desactiva el disparo del enemigo
            damage_player(&game->player); //De ser posible, registra el daño causado al jugador
        }
    }

    //Mientras i sea menor a la cantidad máxima de enemigos permitidos al mismo tiempo
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        Enemy *enemy = &game->enemies[i];

        //Toma un disparo de la lista de disparos de los enemigos, verifica que este se encuentre activo y si ha impactado contra el jugador
        
        if (enemy->active && player_hits_enemy(&game->player, enemy)) {
            //Si la hitbox pertenece a la de un enemigo común
            if (enemy_hitbox_half_width(enemy->type) == 0) {
                enemy->active = 0; //Marca al mismo como inactivo
            }
            damage_player(&game->player); //De ser posible, registra el daño causado al jugador
        }
    }

    //Mientras i sea menor a la cantidad máxima de power-ups activos al mismo tiempo
    for (int i = 0; i < MAX_POWERUPS; ++i) {
        PowerUp *powerup = &game->powerups[i];

        //Toma un power-up de la lista de power-ups, verifica que este se encuentre activo y si ha impactado contra el jugador

        if (powerup->active &&
            (positions_overlap(powerup->position, game->player.position) ||
             projectile_hits_player(powerup->position, &game->player))) {
            //Si el power-up es un dron
            if (powerup->type == POWERUP_DRONE) {
                int previous_drone_timer = game->player.drone_timer;
                game->player.drone_count = MAX_PLAYER_DRONES; //Setea la cantidad de drones del jugador en el máximo permitido 
                game->player.drone_timer = PLAYER_DRONE_DURATION_FRAMES; //Además, reinicia el temporizador de tiempo que el jugador tendrá activos los drones
                set_status_message(game, "DRONES ONLINE");  //Actualiza el mensaje se estatus actual del juego
                if (previous_drone_timer > 0) {
                    game->player.drone_timer = capped_timer_add(previous_drone_timer, PLAYER_DRONE_DURATION_FRAMES);
                    set_status_message(game, "DRONES EXTENDED");
                }
            } else {
                //De lo contrario, actualiza el tipo de arma que el jugador tendrá
                WeaponType previous_weapon = game->player.weapon;
                int previous_weapon_timer = game->player.weapon_timer;
                game->player.weapon = powerup->weapon;
                game->player.weapon_timer = weapon_duration_for_type(powerup->weapon);
                set_status_message(game, weapon_status_message(powerup->weapon));
                if (previous_weapon == powerup->weapon && previous_weapon_timer > 0) {
                    game->player.weapon_timer = capped_timer_add(previous_weapon_timer, weapon_duration_for_type(powerup->weapon));
                    set_status_message(game, "WEAPON EXTENDED");
                }

            }
            //Por último, desactiva el power-up para que el jugador no lo pueda volver a atrapar
            powerup->active = 0;
        }
    }
}
