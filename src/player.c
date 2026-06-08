#include "player.h"

#include "config.h"
#include "projectile.h"

/************************************************************************
* Función: 
    player_init
* Descripción: 
    Inicialización del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Ninguna
*************************************************************************/
void player_init(Player *player)
{   
    //Inicializa las variables del objeto jugador de la siguiente forma
    player->position.x = PLAYER_START_X;    //Coordenada inicial x según definido en config.h
    player->position.y = PLAYER_START_Y;    //Coordenada inicial y según definido en config.h
    player->lives = PLAYER_START_LIVES; //Cantidad de vidas iniciales según definido en config.h
    player->score = 0;  //Puntaje en 0
    player->shot_cooldown = 0;  //Intervalo de frames hasta al siguiente disparo en 0
    player->invulnerable_timer = 0; //Temporizador de imvencibilidad tras recibir un disparo en 0
    player->charge_frames = 0;  //Contador de frames en los que se ha mantenido un disparo cargado en 0
    player->charge_bomb_ready = 0;  //Indicador si la bomba cargada está lista en 0
    player->drone_count = 0;    //Cantidad de drones disponibles en 0
    player->drone_timer = 0;    //Temporizador de frames durante los cuales puede estar activo el dron en cero
    player->weapon_timer = 0;   //Temporizador de frames durante los cuales puede estar activa un arma de power-up en cero
    player->weapon = WEAPON_FRONT;  //Tipo de arma por defecto FRONT
}

/************************************************************************
* Función: 
    clamp_int
* Descripción: 
    Restringe un valor a un rango máximo y mínimo
* Entradas: 
    Número entero que indica valor recibido
    Número entero que indica rango mínimo 
    Número entero que indica rango máximo
* Salidas: 
    Número entero a usar dentro de ese rango
*************************************************************************/
static int clamp_int(int value, int min_value, int max_value)
{   
    //Si el valor ingresado es menor al rango mínimo 
    if (value < min_value) {
        return min_value;   //Retorna el rango mínimo
    }

    //Si el valor ingresado es mayor al rango máximo
    if (value > max_value) {
        return max_value;   //Retorna el rango máximo
    }

    //De lo contrario, si el valor ingresado se encuentra dentro del rango 
    return value;   //Retorna el valor ingresado
}

/************************************************************************
* Función: 
    player_drone_position
* Descripción: 
    Obtiene la posición en la que debe estar ubicado el dron del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Entero que indica el índice del dron a analizar
* Salidas: 
    Vector con coordenadas (x,y) que indican la posición del dron
*************************************************************************/
Vec2i player_drone_position(const Player *player, int drone_index)
{
    //Inicializa un entero de dirección donde, si el índice de dron es cero, este será igual a -1, de lo contrario, 1
    int direction = drone_index == 0 ? -1 : 1;

    /*Inicaliza vector de posición donde:
        Valor x -> Suma de:
                    Coordenada x del jugador
                    Dirección previamente calculada multiplicada por offset/desplazamiento horizontal del dron con respecto al jugador (Definido en config.h)
        Valor y -> Suma de:
                    Coordenada y del jugador
                    Offset/desplazamiento vertical del dron con respecto al jugador (Definido en config.h)*/
    Vec2i position = {
        player->position.x + direction * PLAYER_DRONE_OFFSET_X,
        player->position.y + PLAYER_DRONE_OFFSET_Y
    };

    //Ahora, procede a acomodar los valores dentro de los límites tanto horizontales como verticales del área de juego 
    position.x = clamp_int(position.x, 1, GAME_WIDTH - 2);
    position.y = clamp_int(position.y, 0, GAME_HEIGHT - 1);

    return position;    //Retorna vector obtenido
}

/************************************************************************
* Función: 
    player_center_x
* Descripción: 
    Obtiene la coordenada central x real del jugador
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
* Salidas: 
    Entero que representa coordenada x central de la hitbox del jugador
*************************************************************************/
static int player_center_x(const Player *player)
{   
    //Retorna coordenada x guardada incrementada una vez
    return player->position.x + 1;
}

static void fire_drones(Player *player, Projectile player_shots[], int shot_count)
{
    for (int i = 0; i < player->drone_count; ++i) {
        int direction = i == 0 ? -1 : 1;
        Vec2i drone = player_drone_position(player, i);
        Vec2i shot_position = {drone.x, drone.y - 1};

        if (shot_position.y < 0) {
            continue;
        }

        switch (player->weapon) {
        case WEAPON_SPREAD:
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){direction, -1});
            break;
        case WEAPON_LASER:
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){0, -1});
            projectiles_spawn(player_shots,
                              shot_count,
                              (Vec2i){shot_position.x, shot_position.y - 1},
                              (Vec2i){0, -1});
            break;
        case WEAPON_SIDE:
            projectiles_spawn(player_shots,
                              shot_count,
                              (Vec2i){drone.x, drone.y},
                              (Vec2i){direction, 0});
            break;
        case WEAPON_DOUBLE:
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){0, -1});
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){direction, -1});
            break;
        case WEAPON_FRONT:
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){0, -1});
            break;
        }
    }
}

static void fire_front_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i shot_position = {player_center_x(player), player->position.y - 1};
    Vec2i shot_velocity = {0, -1};
    projectiles_spawn(player_shots, shot_count, shot_position, shot_velocity);
}

static void fire_spread_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i center = {player_center_x(player), player->position.y - 1};
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){-1, -1});
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){1, -1});
}

static void fire_laser_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i first = {player_center_x(player), player->position.y - 1};
    Vec2i second = {player_center_x(player), player->position.y - 2};
    projectiles_spawn(player_shots, shot_count, first, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, second, (Vec2i){0, -1});
}

static void fire_double_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i left = {player->position.x, player->position.y - 1};
    Vec2i right = {player->position.x + 2, player->position.y - 1};
    projectiles_spawn(player_shots, shot_count, left, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, right, (Vec2i){0, -1});
}

static void fire_side_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    Vec2i center = {player_center_x(player), player->position.y - 1};
    Vec2i left = {player->position.x, player->position.y};
    Vec2i right = {player->position.x + 2, player->position.y};

    projectiles_spawn(player_shots, shot_count, center, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, left, (Vec2i){-1, 0});
    projectiles_spawn(player_shots, shot_count, right, (Vec2i){1, 0});
}

static void fire_current_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    switch (player->weapon) {
    case WEAPON_FRONT:
        fire_front_weapon(player, player_shots, shot_count);
        break;
    case WEAPON_SPREAD:
        fire_spread_weapon(player, player_shots, shot_count);
        break;
    case WEAPON_LASER:
        fire_laser_weapon(player, player_shots, shot_count);
        break;
    case WEAPON_DOUBLE:
        fire_double_weapon(player, player_shots, shot_count);
        break;
    case WEAPON_SIDE:
        fire_side_weapon(player, player_shots, shot_count);
        break;
    }
}

void player_update(Player *player, int input_mask, Projectile player_shots[], int shot_count)
{
    player->charge_bomb_ready = 0;

    if (player->invulnerable_timer > 0) {
        player->invulnerable_timer -= 1;
    }
    if (player->weapon_timer > 0) {
        player->weapon_timer -= 1;
        if (player->weapon_timer == 0) {
            player->weapon = WEAPON_FRONT;
        }
    }
    if (player->drone_timer > 0) {
        player->drone_timer -= 1;
        if (player->drone_timer == 0) {
            player->drone_count = 0;
        }
    }

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

    if (player->position.x < 1) {
        player->position.x = 1;
    }
    if (player->position.x >= GAME_WIDTH - 1) {
        player->position.x = GAME_WIDTH - 2;
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

    if (!(input_mask & INPUT_FIRE)) {
        if (player->charge_frames < PLAYER_CHARGE_MAX) {
            player->charge_frames += 1;
        }
        return;
    }

    if (player->charge_frames >= PLAYER_CHARGE_RELEASE_MIN) {
        player->charge_bomb_ready = 1;
        player->charge_frames = 0;
        player->shot_cooldown = PLAYER_SHOT_COOLDOWN;
        return;
    }

    player->charge_frames = 0;

    if (player->shot_cooldown == 0) {
        fire_current_weapon(player, player_shots, shot_count);
        fire_drones(player, player_shots, shot_count);
        player->shot_cooldown = PLAYER_SHOT_COOLDOWN;
    }
}
