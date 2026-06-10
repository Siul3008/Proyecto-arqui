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
// Definición de pasos para el código Konami
enum {
    KONAMI_UP,
    KONAMI_DOWN,
    KONAMI_LEFT,
    KONAMI_RIGHT,
    KONAMI_B,
    KONAMI_A
};
// Secuencia de pasos para el código Konami
static int input_matches_konami_step(int expected_step, int input_mask, int character)
{
    switch (expected_step) {
    case KONAMI_UP:
        return (input_mask & INPUT_UP) != 0;
    case KONAMI_DOWN:
        return (input_mask & INPUT_DOWN) != 0;
    case KONAMI_LEFT:
        return (input_mask & INPUT_LEFT) != 0;
    case KONAMI_RIGHT:
        return (input_mask & INPUT_RIGHT) != 0;
    case KONAMI_B:
        return character == 'b' || character == 'B';
    case KONAMI_A:
        return character == 'a' || character == 'A';
    }

    return 0;
}

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

/************************************************************************
* Función: 
    projectile_inside_radius
* Descripción: 
    Verifica si el enemigo ingresado se encuentra dentro del rango 
        especificado
* Entradas: 
    Puntero a instancia de objeto Enemy (enemigo) a verificar
    Vector con coordenadas (x,y) del centro del radio que se desea 
        verificar
    Número entero que indica el tamaño del radio a verificar, partiendo
        desde el centro de este
* Salidas: 
    1 -> El enemigo se encuentra dentro del rango especificado
    0 -> El enemigo se encuentra fuera del rango especificado
*************************************************************************/
static int enemy_inside_radius(const Enemy *enemy, Vec2i center, int radius)
{
    //Inicializa los difrenciales de x y y en cero
    int dx = 0;
    int dy = 0;

    //Si la coordenada x central del radio es menor a la coordenada x del costado izquierdo del hitbox del enemigo
    if (center.x < enemy_left(enemy)) {
        //El diferencial de x será igual a restar:
        //  La coordenada x del costado izquierdo del hitbox del enemigo
        //  La coordenada x central del radio
        dx = enemy_left(enemy) - center.x;

    //De lo contrario si la coordenada x central del radio es mayor a la coordenada x del costado derecho del hitbox del enemigo
    } else if (center.x > enemy_right(enemy)) {
        //El diferencial de x será igual a restar:
        //  La coordenada x central del radio
        //  La coordenada x del costado derecho del hitbox del enemigo
        dx = center.x - enemy_right(enemy);
    }

    //Si la coordenada y central del radio es menor a la coordenada y del costado superior del hitbox del enemigo
    if (center.y < enemy_top(enemy)) {
        //El diferencial de y será igual a restar:
        //  La coordenada y del costado izquierdo del hitbox del enemigo
        //  La coordenada y central del radio
        dy = enemy_top(enemy) - center.y;

    //De lo contrario si la coordenada y central del radio es mayor a la coordenada y del costado inferior del hitbox del enemigo
    } else if (center.y > enemy_bottom(enemy)) {
        //El diferencial de y será igual a restar:
        //  La coordenada y central del radio
        //  La coordenada y del costado derecho del hitbox del enemigo
        dy = center.y - enemy_bottom(enemy);
    }

    /*Suma los diferenciales previamente calculados.
    El resultado de esta suma se compara con el radio especificado y retorna si el proyectil se encuentra dentro o fuera de este*/
    return dx + dy <= radius;
}

/************************************************************************
* Función: 
    spawn_charge_effects
* Descripción: 
    Se encarga de hacer aparecer los efectos visuales del disparo cargado
        del jugador
* Entradas: 
    Puntero a instancia de objeto Game (juego)
    Vector con coordenadas (x,y) del centro desde el cual van a salir los 
        efectos
* Salidas: 
    Ninguna
*************************************************************************/
static void spawn_charge_effects(GameState *game, Vec2i center)
{
    //Genera los efectos en el punto con las coordenadas proporcionadas
    effect_spawn(game->effects, MAX_EFFECTS, center, EXPLOSION_DURATION);
    
    //Genera los efectos un punto a la izquierda de las coordenadas proporcionadas
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x - 1, center.y}, EXPLOSION_DURATION);
    
    //Genera los efectos un punto a la derecha de las coordenadas proporcionadas
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x + 1, center.y}, EXPLOSION_DURATION);
    
    //Genera los efectos un punto arriba de las coordenadas proporcionadas
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x, center.y - 1}, EXPLOSION_DURATION);
    
    //Genera los efectos un punto debajo de las coordenadas proporcionadas
    effect_spawn(game->effects, MAX_EFFECTS, (Vec2i){center.x, center.y + 1}, EXPLOSION_DURATION);
}

/************************************************************************
* Función: 
    update_charge_shield
* Descripción: 
    Se encarga de hacer aparecer los efectos visuales correspondientes
        a la carga de escudo del jugador (Mientras se carga un disparo, 
        valga la reduncancia), cargado, este podrá actuar como un escudo
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void update_charge_shield(GameState *game)
{
    //Si el número de frames de carga del jugador es menor al requerido para el comportamiento de escudo
    if (game->player.charge_frames < PLAYER_CHARGE_SHIELD_MIN) {
        //Sale inmediatamente de la función
        return;
    }

    //Obtiene las coordenadas correspondientes a la posición donde debe estar la carga
    Vec2i center = player_charge_position(&game->player);

    //Mientras i sea menor al número máximo de disparos enemigos permitidos
    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        //Toma un disparo de la lista de disparos que guarda el objeto Game
        Projectile *shot = &game->enemy_shots[i];

        //Si este disparo está activo, y además se encuentra dentro de un radio igual al del proporcionado por
        //  CHARGE_SHIELD_RADIUS (definido en config.h), partiendo desde el punto central obtenido previamente
        if (shot->active && projectile_inside_radius(shot->position, center, CHARGE_SHIELD_RADIUS)) {
            //Hace que aparezca una explosión, indicando que el disparo ha sido bloqueado
            effect_spawn(game->effects, MAX_EFFECTS, shot->position, EXPLOSION_DURATION);
            //Marca el proyectil como inactivo
            shot->active = 0;
        }
    }
}

/************************************************************************
* Función: 
    apply_charge_bomb
* Descripción: 
    Se encarga del manejo de la bomba del jugador, una vez esta está 
        cargada
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void apply_charge_bomb(GameState *game)
{
    //Si el estado de carga de la bomba no indica que esta está lista
    if (!game->player.charge_bomb_ready) {
        //Sale de la función
        return;
    }

    //Obtiene la posición desde donde debe salir la bomba
    Vec2i center = player_charge_position(&game->player);
    //Genera un efecto visual en esas coordenadas
    spawn_charge_effects(game, center);

    //Mientras i sea menor al número máximo de disparos enemigos permitidos
    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        //Toma un disparo de la lista de disparos que guarda el objeto Game
        Projectile *shot = &game->enemy_shots[i];

        //Si este disparo está activo, y además se encuentra dentro de un radio igual al del proporcionado por
        //  CHARGE_BOMB_RADIUS (definido en config.h), partiendo desde el punto central obtenido previamente
        if (shot->active && projectile_inside_radius(shot->position, center, CHARGE_BOMB_RADIUS)) {
            //Marca el proyectil como inactivo
            shot->active = 0;
        }
    }

    //Mientras i sea menor al número máximo de enemigos permitidos
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        //Toma un enemigo de la lista de enemigos que guarda el objeto Game
        Enemy *enemy = &game->enemies[i];

        //Si el enemigo está activo o este no se encuentra dentro de un radio igual al del proporcionado por
        //  CHARGE_BOMB_RADIUS (definido en config.h), partiendo desde el punto central obtenido previamente
        if (!enemy->active || !enemy_inside_radius(enemy, center, CHARGE_BOMB_RADIUS)) {
            //Va al siguiente ciclo, es decir, al siguiente enemigo
            continue;
        }

        //De lo contrario, le resta a la vida del enemigo, el daño causado por la bomba (valor definido en charge.h)
        enemy->health -= CHARGE_BOMB_DAMAGE;
        //Si la vida del enemigo es menor o igual a cero
        if (enemy->health <= 0) {
            //Genera un efecto de explosión, para indicar que este ha sido derrotado
            effect_spawn(game->effects, MAX_EFFECTS, enemy->position, EXPLOSION_DURATION);
            enemy->active = 0;  //Marca el enemigo como inactivo
            //Le suma al jugador el puntaje correspondiente al tipo de enemigo que este era
            game->player.score += score_for_enemy_type(enemy->type);  
        }
    }
}

/************************************************************************
* Función: 
    reset_run
* Descripción: 
    Reestablece el estado de la partida, es decir, la vuelve a empezar}
        desde cero
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void reset_run(GameState *game)
{
    player_init(&game->player); //Incializa desde cero el estado del jugador
    projectiles_clear(game->player_shots, MAX_PLAYER_SHOTS);    //Reestablece el estado de todos los proyectiles del jugador
    effect_clear(game->effects, MAX_EFFECTS);   //Restablece el estado de todos los efectos visuales
    projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);  //Reestablece el estado de todos los proyectiles enemigos
    powerups_clear(game->powerups, MAX_POWERUPS);   //Reestablece el estado de todos los Power-ups de la partida
    enemies_clear(game->enemies, MAX_ENEMIES);  //Reestablece el estado de todos los enemigos

    game->frame = 0;    //Reestablece el contador de frames
    game->level = 1;    //Reestablece el contador de niveles
    game->boss_count = 0;  //Reestablece el contador de jefes
    game->next_boss_score = BOSS_SCORE_INTERVAL;    //Reestablece el cálculo de puntos necesarios para llegar al siguiente jefe
    game->next_extra_life_score = EXTRA_LIFE_SCORE_INTERVAL;    //Reestablece el cálculo de puntos necesarios para llegar a tener una vida extra
    game->wave_spawned = 0; //Reestablece el contador de olas generadas
    game->next_spawn_frame = 0; //Reestablece el control de cual debería ser el siguiente frame donde haya generación 
    game->phase = LEVEL_PHASE_NORMAL;   //Reestablece el indicador de fase de partida actual
    game->status_message[0] = '\0';     //Manda un caracter nulo al buffer que presenta en pantalla el estado del juego
    game->status_message_timer = 0;     //Reestablece el contador de frames que debería estar en pantalla el mensaje de estadod e la partida
    game->name_input[0] = '\0';     //Manda un caracter nulo al buffer donde el jugador ingresa su nombre al finalizar la partida
    game->name_length = 0;  //Establece en cero el contador de caracteres dwl nombre ingresado
    game->score_recorded = 0;   //Establece en cero el puntaje obtenido al finalizar la partida       
    game->konami_step = 0;

    if (game->konami_unlocked) {
        game->player.lives = KONAMI_CHEAT_LIVES;
        game->player.drone_count = MAX_PLAYER_DRONES;
        game->player.drone_timer = KONAMI_DRONE_DURATION_FRAMES;
    }
}

/************************************************************************
* Función: 
    set_status_message
* Descripción: 
    Actualiza el mensaje de estado de la partida
* Entradas: 
    Puntero a instancia de objeto Game (juego)
    Puntero a cadena de caracteres que contiene el mensaje
* Salidas: 
    Ninguna
*************************************************************************/
static void set_status_message(GameState *game, const char *message)
{
    strncpy(game->status_message, message, STATUS_MESSAGE_LENGTH - 1);  //Manda la cadena de caracteres al buffer destinado para que sea presentado en pantalla
    game->status_message[STATUS_MESSAGE_LENGTH - 1] = '\0'; //Manda un caracter nulo al último caracter del buffer
    game->status_message_timer = STATUS_MESSAGE_DURATION;   //Establece el tiempo que debe durar este mensaje en pantalla según el valor definido en config.h
}


/************************************************************************
* Función: 
    activate_konami_cheat
* Descripción:
    Activa el código Konami, otorgando al jugador una cantidad de vidas y drones, además de actualizar el mensaje de estado del juego para indicar que el código ha sido activado
* Entradas:
    Puntero a instancia de objeto Game (juego)
* Salidas:
    Ninguna
*************************************************************************/
static void activate_konami_cheat(GameState *game)
{
    game->konami_unlocked = 1;
    game->konami_step = 0;
    game->player.lives = KONAMI_CHEAT_LIVES;
    game->player.drone_count = MAX_PLAYER_DRONES;
    game->player.drone_timer = KONAMI_DRONE_DURATION_FRAMES;
    set_status_message(game, "KONAMI MODE");
}

/************************************************************************
* Función: 
    update_konami_code 
* Descripción:
    Verifica si el jugador ha ingresado correctamente la secuencia del código Konami, y en caso de que así sea, activa el código Konami
* Entradas:
    Puntero a instancia de objeto Game (juego)
    Máscara de entrada actual del jugador
* Salidas:
    Ninguna
*************************************************************************/
static void update_konami_code(GameState *game, int input_mask)
{
    static const int sequence[] = {
        KONAMI_UP,
        KONAMI_UP,
        KONAMI_DOWN,
        KONAMI_DOWN,
        KONAMI_LEFT,
        KONAMI_RIGHT,
        KONAMI_LEFT,
        KONAMI_RIGHT,
        KONAMI_B,
        KONAMI_A
    };
    int character = input_last_character();
    int sequence_length = (int)(sizeof(sequence) / sizeof(sequence[0]));

    if (character < 0) {
        return;
    }

    if (input_matches_konami_step(sequence[game->konami_step], input_mask, character)) {
        game->konami_step += 1;
        if (game->konami_step >= sequence_length) {
            activate_konami_cheat(game);
        }
        return;
    }

    game->konami_step = input_matches_konami_step(sequence[0], input_mask, character) ? 1 : 0;
}

/************************************************************************
* Función: 
    update_extra_life
* Descripción: 
    Actualiza si el jugador debe recibir una vida extra
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void update_extra_life(GameState *game)
{
    //Mientras el puntaje del jugador sea mayor o igual al siguiente puntaje necesario para que este reciba una vida extra
    while (game->player.score >= game->next_extra_life_score) {
        //Si la cantidad de vidas del jugador es menor a la cantidad máxima de vidas que este puede tener
        if (game->player.lives < PLAYER_MAX_LIVES) {
            game->player.lives += 1;    //Se le suma una vida al jugador
            set_status_message(game, "LIFE RECOVERED"); //Actualiza el estatus del juego para indicar que se le ha otrogado una vida extra
        }

        //Se procede a sumarle el intervalo entre vidas extras (definido en config.h), al indicador de siguiente puntaje necesario para recibir una vida extra
        game->next_extra_life_score += EXTRA_LIFE_SCORE_INTERVAL; 
    }
}

/************************************************************************
* Función: 
    begin_name_entry
* Descripción: 
    Una vez el jugador pierde todas sus vidas, se pasa a que este ingrese
        su nombre para guardar el puntaje que obtuvo, esta función es la 
        encargada de preparar el buffer y decirle al juego que debe ir 
        a la pantalla pertinente
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void begin_name_entry(GameState *game)
{
    game->name_input[0] = '\0'; //Manda un caracter nulo al final del buffer destinado para la entrada del nombre
    game->name_length = 0;  //Establece la longitud del nombre ingresado en 0
    game->screen = GAME_SCREEN_NAME_ENTRY;  //Le indica al juego que debe pasar a la pantalla pertinente
}

/************************************************************************
* Función: 
    submit_high_score
* Descripción: 
    Procesa el nombre ingresado por el jugador para guardarlo en la tabla 
        histórica de puntuaciones, además, le indica al juego que debe 
        cambiar a la pantalla de "GAME OVER"
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void submit_high_score(GameState *game)
{
    //Define objeto entry 
    HighScoreEntry entry;

    //Si la longitud del nombre ingresado es igual a cero
    if (game->name_length == 0) {
        //Copia la palabra "PLAYER" al buffer donde se ingresa el nombre que el jugador ha ingresado
        strncpy(game->name_input, "PLAYER", PLAYER_NAME_MAX_LENGTH);
        game->name_input[PLAYER_NAME_MAX_LENGTH] = '\0';  //Añade un caracter nulo al final de dicho buffer
    }

    //Se copia el buffer donde el usuario ha ingresado su nombre al destinado para guardar el mismo
    strncpy(entry.name, game->name_input, PLAYER_NAME_MAX_LENGTH);
    entry.name[PLAYER_NAME_MAX_LENGTH] = '\0';  //Añade un caracter nulo al final de este
    entry.score = game->player.score;   //Guarda el puntaje final del jugador
    entry.rank = game->level;           //Guarda el nivel hasta donde llegó el jugador
    entry.bosses = game->boss_count;    //Guarda la cantidad de jefes que este logró derrotar

    highscores_add(game->high_scores, MAX_HIGH_SCORES, entry);  //Añade los datos del objeto entry a la tabla con los puntajes guardados
    highscores_save(game->high_scores, MAX_HIGH_SCORES, HIGHSCORE_FILE_NAME);   //Guarda dicha tabla

    game->score_recorded = 1; //Marca que ya se ha guardado el puntaje del jugador
    game->screen = GAME_SCREEN_GAME_OVER;   //Le indica al juego que debe pasar a la pantalla de "GAME OVER"
}

/************************************************************************
* Función: 
    update_name_entry
* Descripción: 
    Actualiza el estado en el que se encuentra el buffer de entrada 
        destinado para el ingreso del nombre del jugador
* Entradas: 
    Puntero a instancia de objeto Game (juego)
    Indicador de si alguna tecla ha sido presionada por el usuario
* Salidas: 
    Ninguna
*************************************************************************/
static void update_name_entry(GameState *game, int input_mask)
{
    //Recupera el último caracter presente en el buffer de entrada
    int character = input_last_character();
    
    //Si se ha presionado una tecla y esta corresponde a START (Enter)
    if (input_mask & INPUT_START) {
        submit_high_score(game); //Procesa y guarda el nombre del jugador y su puntaje
        return; //Sale de la función
    }

    //Si se ha presionado una tecla, esta corresponde a BACKSPACE, y además no se ha ingresado ningún caracter
    if ((input_mask & INPUT_BACKSPACE) && game->name_length > 0) {        
        game->name_length -= 1; //Establece la longitud del nombre ingresado en -1
        game->name_input[game->name_length] = '\0'; //Manda uin caracter nulo al último caracter del buffer
        return; //Sale de la función
    }

    /*Verifica:
        1. Si el valor almacenado en la variable de caracter (character) es mayor a cero
        2. Si este es alfanumérico
        3. Si la longitud del nombre hasta el momento es menor a la longitud máxima permitida (definido en config.h)*/
    if (character >= 0 && character <= 255 && isalnum((unsigned char)character) && game->name_length < PLAYER_NAME_MAX_LENGTH) {
        game->name_input[game->name_length] = (char)toupper((unsigned char)character); //Transforma el último caracter ingresado, en caso de ser una letra, a mayúscula
        game->name_length += 1; //Le suma uno al contador de caracteres que tiene el nombre
        game->name_input[game->name_length] = '\0'; //Se le agrega un caracter nulo al final del buffer
    }
}

/************************************************************************
* Función: 
    update_wave_spawning
* Descripción: 
    Actualiza el estado en el que se encuentran las olas del juego
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void update_wave_spawning(GameState *game)
{
    //Si la fase en la que se encuentra la partida no es fase de ola normal, es decir, es nivel de jefe
    if (game->phase != LEVEL_PHASE_NORMAL) {
        return; //Sale de la función
    }

    //Si el puntaje el jugador es meyor o igual al requerido para el siguiente jefe
    if (game->player.score >= game->next_boss_score) {
        enemies_clear(game->enemies, MAX_ENEMIES);  //Limpia todos los enemigos existentes en pantalla
        projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);  //Limpia todos los proyectiles que hay en pantalla
        enemies_spawn_boss(game->enemies, MAX_ENEMIES, boss_type_for_count(game->boss_count));  //Genera el jefe corresdpondiente en pantalla
        game->phase = LEVEL_PHASE_BOSS; //Actualiza la fase de la partida a nivel de jefe
        return; //Sale de la función
    }

    //Si el número de frame en el que se encuentra el juego es mayor o igual al siguiente frame donde se deben generar enemigos
    if (game->frame >= game->next_spawn_frame) {
        //Inicializa objeto EnemyType al obtener el tipo de enemigo correspondiente a la ola generada y el nivel en el que se encuentra el juego 
        EnemyType type = enemy_type_for_wave(game->level, game->wave_spawned);
        enemies_spawn(game->enemies, MAX_ENEMIES, spawn_x_for_wave(game), type);  //Genera a los enemigos en pantalla
        game->wave_spawned += 1;    //Incrementa una vez el contador de olas generadas

        /*Actualiza el indicador de siguiente frame de generación, esto al sumar:
            Número de frame actual
            Intervalo de generación de enemigos (Según nivel actual del juego)*/
        game->next_spawn_frame = game->frame + enemy_spawn_interval_for_rank(game->level);  
    }
}

/************************************************************************
* Función: 
    update_level_progression
* Descripción: 
    Actualiza el estado de progreso de los niveles del juego
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void update_level_progression(GameState *game)
{
    //Si la fase actual del juego es fase de jefe y si no hay ningún jefe en la lista de enemigos que guarda el juego
    if (game->phase == LEVEL_PHASE_BOSS && !enemies_has_boss(game->enemies, MAX_ENEMIES)) {
        game->boss_count += 1;  //Incrementa una vez el contador de jefes vencidos
        game->next_boss_score += next_boss_score_after_count(game->boss_count); //Le otorga al jugador el puntaje correspondiente al tipo de jefe que este era
        game->wave_spawned = 0; //Reinicia el contador de olas generadas
        /*Actualiza el indicador de siguiente frame de generación, esto al sumar:
            Número de frame actual
            Número de descanso de frames entre oleadas (Definido en config.h)*/
        game->next_spawn_frame = game->frame + WAVE_BREAK_FRAMES;
        game->phase = LEVEL_PHASE_NORMAL;   //Establece el indicador de fase de juego en normal
        projectiles_clear(game->enemy_shots, MAX_ENEMY_SHOTS);  //Limpia todos los proyectiles en pantalla
    }
}

/************************************************************************
* Función: 
    game_init
* Descripción: 
    Incicialización del juego
* Entradas: 
    Puntero a instancia de objeto Game (juego)
* Salidas: 
    Ninguna
*************************************************************************/
void game_init(GameState *game)
{
    game->konami_step = 0;
    game->konami_unlocked = 0;
    reset_run(game);  //Reestablece todos los valores necesarios para el juego 
    highscores_load(game->high_scores, MAX_HIGH_SCORES, HIGHSCORE_FILE_NAME);   //Carga la tabla de puntajes
    game->running = 1;  //Actualiza el estado de que el juego está corriendo, es decir, lo "activa"
    game->screen = GAME_SCREEN_MENU;    //Establece la pantalla en la que debe estar el juego, en este caso, el menú de inicio
    game->previous_screen = GAME_SCREEN_MENU;   //Establece la pantalla en la que estuvo el juego anteriormente, en este caso, el menú de inicio
}

/************************************************************************
* Función: 
    game_init
* Descripción: 
    Actualización general del estado del juego
* Entradas: 
    Puntero a instancia de objeto Game (juego)
    Indicador de si alguna tecla ha sido presionada por el usuario
* Salidas: 
    Ninguna
*************************************************************************/
void game_update(GameState *game, int input_mask)
{   //Si la pantalla en la que se encuentra el juego es en la que el jugador ingresa su nombre
    if (game->screen == GAME_SCREEN_NAME_ENTRY) {
        update_name_entry(game, input_mask);    //Actualiza el estado del ingreso de nombre
        return; //Sale de la función
    }

    update_konami_code(game, input_mask);

    //Si se ha presionado una tecla y esta ha sido QUIT (Q)
    if (input_mask & INPUT_QUIT) {
        game->running = 0;  //Establece el estado del juego como inactivo, es decir, ya no está corriendo
        return; //Sale de la función
    }

    //Si la pantalla en la que se encuentra el juego es la de ayuda
    if (game->screen == GAME_SCREEN_HELP) {
        //Si se ha presionado una tecla y esta es HELP (H), START (Enter) o PAUSE (P)
        if (input_mask & (INPUT_HELP | INPUT_START | INPUT_PAUSE)) {
            //Regresa a la pantalla en la que el juego estaba anteriormente
            game->screen = game->previous_screen;
        }
        return; //Sale de la función
    }

    //Si se ha presionado una tecla y esta ha sido HELP (H)
    if (input_mask & INPUT_HELP) {
        game->previous_screen = game->screen; //Guarda la pantalla en la que estaba el juego
        game->screen = GAME_SCREEN_HELP;    //Establece la pantalla del juego en la pantalla de ayuda
        return; //Sale de la función
    }   

    //Si el juego se encuentra en la pantalla de menú principal
    if (game->screen == GAME_SCREEN_MENU) {
        //Si se ha presionado una tecla y esta ha sido START (Enter)
        if (input_mask & INPUT_START) {
            reset_run(game);    //Reestablece el estado del juego
            game->screen = GAME_SCREEN_PLAYING; //Establece la pantalla del juego en la que indica que la partida está en curso
        }
        return; //Sale de la función 
    }

    //Si el juego se encuentra en la pantalla de GAME OVER
    if (game->screen == GAME_SCREEN_GAME_OVER) {
        //Si se ha presionado una tecla y esta ha sido RESTART (R)
        if (input_mask & INPUT_RESTART) {
            game->konami_unlocked = 0;   //Desactiva el código Konami, en caso de que este haya sido activado
            reset_run(game);    //Reestablece el estado del juego
            game->screen = GAME_SCREEN_PLAYING; //Establece la pantalla del juego en la que indica que la partida está en curso
        }
        return; //Sale de la función 
    }

    //Si el juego se encuentra en la pantalla de pausa
    if (game->screen == GAME_SCREEN_PAUSED) {
        //Si se ha presionado una tecla y esta ha sido PAUSE (P)
        if (input_mask & INPUT_PAUSE) {
            game->screen = GAME_SCREEN_PLAYING; //Establece la pantalla del juego en la que indica que la partida está en curso
        }
        return; //Sale de la función 
    }

    //Si se ha presionado una tecla y esta ha sido PAUSE (P)
    if (input_mask & INPUT_PAUSE) {
        game->screen = GAME_SCREEN_PAUSED;  //Establece la pantalla del juego en la pantalla de pausa
        return; //Sale de la función
    }

    player_update(&game->player, input_mask, game->player_shots, MAX_PLAYER_SHOTS); //Actualiza el estado del jugador
    apply_charge_bomb(game);    //Actualiza el estado de la bomba del jugador
    update_wave_spawning(game); //Actualiza el estado de las olas generadas

    projectiles_update(game->player_shots,
                       MAX_PLAYER_SHOTS,
                       game->frame,
                       PLAYER_SHOT_MOVE_INTERVAL);  //Actualiza el estado de los proyectiles del jugador
    projectiles_update(game->enemy_shots,
                       MAX_ENEMY_SHOTS,
                       game->frame,
                       enemy_shot_move_interval_for_wave(game->level)); //Actualiza el estado de los proyetiles de los enemigos
    powerups_update(game->powerups,
                    MAX_POWERUPS,
                    game->frame,
                    POWERUP_MOVE_INTERVAL); //Actualiza el estado de los Power-Ups
    enemies_update(game->enemies,
                   MAX_ENEMIES,
                   game->enemy_shots,
                   MAX_ENEMY_SHOTS,
                   game->frame,
                   enemy_move_interval_for_wave(game->level));  //Actualiza el estado de los enemigos
    update_charge_shield(game); //Actualiza el estado del escudo cargadod el jugador
    collisions_update(game);    //Actualiza el estado de las colisiones del juego
    update_extra_life(game);    //Verifica si al jugador se le debe otorgar una vida extra
    game->level = rank_for_score(game->player.score);   //Actualiza el nivel del juego según el puntaje actual del jugador
    update_level_progression(game); //Actualiza el estado de progresión del nivel actual

    //Si el temporizador de frames que indica cuantos frames debe permanecer el mensaje de estado en pantalla es mayor a cero
    if (game->status_message_timer > 0) {
        game->status_message_timer -= 1; //Le resta un frame de tiempo disponible
    }

    //Si la cantidad de vidas del jugador es menor o igual que cero
    if (game->player.lives <= 0) {
        //Si el nombre del jugador no ha sido guardado
        if (!game->score_recorded) {
            begin_name_entry(game); //Prepara todo para el ingreso del nombre del mismo
        
        //De lo contrario
        } else {
            game->screen = GAME_SCREEN_GAME_OVER; //Establece la pantalla actual del juego en GAME OVER
        }
    }

    game->frame += 1;   //Incrementa en 1 el indicador de frame actual
    effect_update(game->effects, MAX_EFFECTS);  //Actualiza los efectos visuales en pantalla
}
