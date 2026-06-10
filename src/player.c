#include "player.h"

#include "config.h"
#include "projectile.h"
#include "sound.h"

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

/************************************************************************
* Función: 
    fire_drones
* Descripción: 
    Función encargada de hacer que los drones del jugador disparen
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_drones(Player *player, Projectile player_shots[], int shot_count)
{
    //Mientras i sea menor a la cantidad de drones con los que cuenta el jugador
    for (int i = 0; i < player->drone_count; ++i) {
        /*Inicializa la variable de dirección y evalúa si i es igual a cero
            Si es igual a cero -> direction = -1
            Si no es igual a cero -> direction = 1*/
        int direction = i == 0 ? -1 : 1;

        //Obtiene las coordenadas (x,y) del dron
        Vec2i drone = player_drone_position(player, i);
        //Define las coordenadas del disparo del dron, haciendo que este salga del costado superior del mismo
        Vec2i shot_position = {drone.x, drone.y - 1};

        //Si la posición vertical del proyectil es menor a cero, significa que se encutra fuera del área de juego
        if (shot_position.y < 0) {
            //Pasa a la siguiente iteración, es decir, para a procesar el siguiente dron (en caso de haber más de uno)
            continue;
        }

        //Toma el tipo de arma con la que cuenta el jugador
        switch (player->weapon) {
        //Si es tipo SPREAD
        case WEAPON_SPREAD:
            /*Pasa los siguientes parámetros a la función projectiles_spawn:
                Arreglo de instancias de objeto Projectile (proyectiles)
                Número entero que indica la cantidad de proyectiles activos en el juego
                Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
                Vector con las velocidades "x" y "y" correspondientes, donde:
                    Velocidad x = direction (previamente calculado)
                    Velocidad y = -1 (para que se mueva hacia "arriba")*/
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){direction, -1});
            break;
        //Si es tipo LASER
        case WEAPON_LASER:
            /*Pasa los siguientes parámetros a la función projectiles_spawn:
                Arreglo de instancias de objeto Projectile (proyectiles)
                Número entero que indica la cantidad de proyectiles activos en el juego
                Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
                Vector con las velocidades "x" y "y" correspondientes, donde:
                    Velocidad x = 0 (sin desplazamiento horizontal)
                    Velocidad y = -1 (para que se mueva hacia "arriba")*/
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){0, -1});

            //Además, genera otro proyectil directamente arriba del que se acaba de generar, esto para lograr el efecto de que el laser
            //  es más efectivo (De manera visual lo hace ver más largo en comparación a un proyectil común)

            /*Pasa los siguientes parámetros a la función projectiles_spawn:
                Arreglo de instancias de objeto Projectile (proyectiles)
                Número entero que indica la cantidad de proyectiles activos en el juego
                Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
                Vector con las velocidades "x" y "y" correspondientes, donde:
                    Velocidad x = 0 (sin desplazamiento horizontal)
                    Velocidad y = -1 (para que se mueva hacia "arriba")*/
            projectiles_spawn(player_shots, shot_count, (Vec2i){shot_position.x, shot_position.y - 1}, (Vec2i){0, -1});
            break;
        //Si es tipo SIDE
        case WEAPON_SIDE:
            /*Pasa los siguientes parámetros a la función projectiles_spawn:
                Arreglo de instancias de objeto Projectile (proyectiles)
                Número entero que indica la cantidad de proyectiles activos en el juego
                Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
                Vector con las velocidades "x" y "y" correspondientes, donde:
                    Velocidad x = direction (previamente calculado)
                    Velocidad y = 0 (sin desplazamiento vertical)*/
            projectiles_spawn(player_shots, shot_count, (Vec2i){drone.x, drone.y}, (Vec2i){direction, 0});
            break;
        //Si es tipo DOUBLE
        case WEAPON_DOUBLE:
            //Genera los proyectiles necesarios, tomando en cuenta las velocidades respectivas para lograr un movimiento solo vertical:
            //  Velocidad x = 0 (sin desplazamiento horizontal)
            //  Velocidad y = -1 (para que se mueva hacia "arriba")
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){0, -1});

            /*Pasa los siguientes parámetros a la función projectiles_spawn:
                Arreglo de instancias de objeto Projectile (proyectiles)
                Número entero que indica la cantidad de proyectiles activos en el juego
                Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
                Vector con las velocidades "x" y "y" correspondientes, donde:
                  Velocidad x = direction (previamente calculado)
                  Velocidad y = -1 (para que se mueva hacia "arriba")*/
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){direction, -1});
            break;
        //Si es tipo FRONT
        case WEAPON_FRONT:
            /*Pasa los siguientes parámetros a la función projectiles_spawn:
                Arreglo de instancias de objeto Projectile (proyectiles)
                Número entero que indica la cantidad de proyectiles activos en el juego
                Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
                Vector con las velocidades "x" y "y" correspondientes, donde:
                Velocidad x = 0 (sin desplazamiento horizontal)
                Velocidad y = -1 (para que se mueva hacia "arriba")*/
            projectiles_spawn(player_shots, shot_count, shot_position, (Vec2i){0, -1});
            break;
        }
    }
}

/************************************************************************
* Función: 
    fire_front_weapon
* Descripción: 
    Función encargada de disparar proyectiles con arma tipo FRONT
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_front_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    /*Obtiene las coordenadas (x,y) desde las cuales debe salir el disparo, donde:
        Coordenada x = Coordenada x central de hitbox del jugador
        Coordenada y = Coordenada y del jugador + 1 (esto para que el disparo salga directamente arriba del jugador)*/
    Vec2i shot_position = {player_center_x(player), player->position.y - 1};
    /*Obtiene las velocidades correspondientes a cada eje donde:
        Velocidad x = 0 (sin desplazamiento horizontal)
        Velocidad y = -1 (desplazamiento vertical hacia arriba)*/
    Vec2i shot_velocity = {0, -1};

    /*Pasa los siguientes parámetros a la función projectiles_spawn:
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
            Vector con las velocidades "x" y "y" correspondientes (previamente calculado)*/
    projectiles_spawn(player_shots, shot_count, shot_position, shot_velocity);
}

/************************************************************************
* Función: 
    fire_spread_weapon
* Descripción: 
    Función encargada de disparar proyectiles con arma tipo SPREAD
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_spread_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    /*Obtiene las coordenadas (x,y) desde las cuales deben salir los disparos, donde:
        Coordenada x = Coordenada x central de hitbox del jugador
        Coordenada y = Coordenada y del jugador + 1 (esto para que los disparos salgan directamente arriba del jugador)*/
    Vec2i center = {player_center_x(player), player->position.y - 1};

    /*Pasa los siguientes parámetros a la función projectiles_spawn (una vez por proyectil):
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (En este caso, se desea que todos aparezcan desde el centro)
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que los proyectiles se dispersen, se colocan los siguientes parámetros en dicho vector:
                    (0,-1) -> Directamente hacia arriba
                    (-1, -1) -> Diagonal hacia arriba y a la izquierda
                    (1, -1) -> Diagonal hacia arriba y a la derecha)*/
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){-1, -1});
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){1, -1});
}

/************************************************************************
* Función: 
    fire_laser_weapon
* Descripción: 
    Función encargada de disparar proyectiles con arma tipo LASER
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_laser_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    //Obtiene las coordenadas desde las cuales debe salir el primer disparo
    Vec2i first = {player_center_x(player), player->position.y - 1};

    //Además, genera otro proyectil directamente arriba del que se acaba de generar, esto para lograr el efecto de que el laser
    //  es más efectivo (De manera visual lo hace ver más largo en comparación a un proyectil común)
    //Por tanto, obtiene las coordenadas desde las cuales debe salir el siguiente disparo
    Vec2i second = {player_center_x(player), player->position.y - 2};

    /*Pasa los siguientes parámetros a la función projectiles_spawn:
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
            Vector con las velocidades "x" y "y" correspondientes (previamente calculado)*/
    projectiles_spawn(player_shots, shot_count, first, (Vec2i){0, -1});

    /*Pasa los siguientes parámetros a la función projectiles_spawn:
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (previamente calculado)
            Vector con las velocidades "x" y "y" correspondientes, donde:
                Velocidad x = 0 (sin desplazamiento horizontal)
                Velocidad y = -1 (para que se mueva hacia "arriba")*/    
    projectiles_spawn(player_shots, shot_count, second, (Vec2i){0, -1});
}

/************************************************************************
* Función: 
    fire_double_weapon
* Descripción: 
    Función encargada de disparar proyectiles con arma tipo DOUBLE
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_double_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    //Como el arma DOUBLE hace que los disparos del jugador salgan desde los lados de este, se obtienen las coordenadas desde las cuales deben salir todos los proyectiles
    Vec2i left = {player->position.x, player->position.y - 1};  //Costado izquierdo
    Vec2i right = {player->position.x + 2, player->position.y - 1}; //Costado derecho

    /*Pasa los siguientes parámetros a la función projectiles_spawn (una vez por proyectil):
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (En este caso, se desea que todos aparezcan desde el centro)
            Vector con las velocidades "x" y "y" correspondientes:
                    (0,-1) -> Directamente hacia arriba*/
    projectiles_spawn(player_shots, shot_count, left, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, right, (Vec2i){0, -1});
}

/************************************************************************
* Función: 
    fire_side_weapon
* Descripción: 
    Función encargada de disparar proyectiles con arma tipo SIDE
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_side_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    //Como el arma SIDE hace que los disparos del jugador sean complementados por dos adicionales que salen hacia los lados de este, se obtienen 
    //  las coordenadas desde donde deben salir todos los proyectiles
    Vec2i center = {player_center_x(player), player->position.y - 1};   //Coordenada central
    Vec2i left = {player->position.x, player->position.y};  //Coordenada costado izquierdo
    Vec2i right = {player->position.x + 2, player->position.y}; //Coordenada costado derecho

    /*Pasa los siguientes parámetros a la función projectiles_spawn (una vez por proyectil):
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (En este caso, se desea que todos aparezcan desde el centro)
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que los proyectiles se dispersen, se colocan los siguientes parámetros en dicho vector:
                    (0,-1) -> Directamente hacia arriba
                    (-1, 0) -> Hacia la izquierda
                    (1, 0) -> Hacia la derecha*/
    projectiles_spawn(player_shots, shot_count, center, (Vec2i){0, -1});
    projectiles_spawn(player_shots, shot_count, left, (Vec2i){-1, 0});
    projectiles_spawn(player_shots, shot_count, right, (Vec2i){1, 0});
}

/************************************************************************
* Función: 
    fire_current_weapon
* Descripción: 
    Función encargada de disparar proyectiles con el arma del jugador 
        según su tipo
* Entradas: 
    Puntero a instancia de objeto Player (jugador)
    Arreglo con instancias de objeto Projectile (proyectiles) del jugador
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_current_weapon(Player *player, Projectile player_shots[], int shot_count)
{
    //Toma el tipo de arma con la que cuenta el jugador
    //  Según el tipo de arma que sea, llama la función correspondiente a ese tipo de arma
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

/************************************************************************
* Función: 
   enemies_update
* Descripción: 
    Actualiza el estado de todos los enemigos en el juego
* Entradas:
    Puntero a instancia de objeto Player (jugador)
    Indicador de si alguna tecla ha sido presionada por el usuario
    Arreglo de instancias de objeto Projectile (proyectiles) (En este
        caso del jugador)
    Número entero que indica la cantidad de proyectiles activos en el 
        juego
* Salidas: 
    Ninguna
*************************************************************************/
void player_update(Player *player, int input_mask, Projectile player_shots[], int shot_count)
{
    //Inicialmente, establece que la bomba del jugador no está cargada todavía
    player->charge_bomb_ready = 0;

    //-------------------------- MANEJO TEMPORIZADORES QUE AFECTAN AL JUGADOR --------------------------

    //Si el temporizador de invulnerabilidad del jugador tras perder una vida, es mayor a cero
    if (player->invulnerable_timer > 0) {
        //Lo decrementa una vez
        player->invulnerable_timer -= 1;
    }

    //Si el temporizador de posesión de arma especial del jugador es mayor a cero
    if (player->weapon_timer > 0) {
        //Lo decrementa una vez
        player->weapon_timer -= 1;

        //Además, si este temorizador ya ha llegado a cero
        if (player->weapon_timer == 0) {
            //Le asigna al jugador el arma base, en este caso, FRONT
            player->weapon = WEAPON_FRONT;
        }
    }

    //Si el temporizador de posesión de drones del jugador es mayor a cero
    if (player->drone_timer > 0) {
        //Lo decrementa una vez
        player->drone_timer -= 1;
        
        //Además, si este temorizador ya ha llegado a cero
        if (player->drone_timer == 0) {
            //Establece que el jugador ya no cuenta con drones activos
            player->drone_count = 0;
        }
    }
    
    //Si el intervalo restante hasta el siguiente disparo del jugador es mayor a uno
    if (player->shot_cooldown > 0) {
        //Lo decrementa una vez
        player->shot_cooldown -= 1;
    }

    //-------------------------- MANEJO ENTRADAS DE MOVIMIENTO DEL JUGADOR --------------------------

    //Si se ha presionado una tecla y esta es LEFT (A) o (Flecha izquierda)
    if (input_mask & INPUT_LEFT) {
        //Modifica la coordenada x, haciendo que este se mueva a la izquierda
        player->position.x -= 1;
    }

    //Si se ha presionado una tecla y esta es RIGHT (D) o (Flecha derecha)
    if (input_mask & INPUT_RIGHT) {
        //Modifica la coordenada x, haciendo que este se mueva a la derecha
        player->position.x += 1;
    }

    //Si se ha presionado una tecla y esta es UP (W) o (Flecha arriba)
    if (input_mask & INPUT_UP) {
        //Modifica la coordenada y, haciendo que este se mueva hacia arriba
        player->position.y -= 1;
    }

    //Si se ha presionado una tecla y esta es UP (W) o (Flecha arriba)
    if (input_mask & INPUT_DOWN) {
        player->position.y += 1;
    }

    //-------------------------- MANEJO POSICIÓN EN ÁREA DE JUEGO DEL JUGADOR --------------------------
    
    //Si la coordenada x del jugador es menor a 1
    if (player->position.x < 1) {
        //La establece en 1 
        player->position.x = 1;
    }

    //Si la coordenada x del jugador es mayor o igual a el ancho total del área de juego decrementado una vez
    if (player->position.x >= GAME_WIDTH - 1) {
        //La establece en el ancho total del área de juego decrementado 2 veces
        player->position.x = GAME_WIDTH - 2;
    }

    //Si la coordenada y del jugador es menor a 0
    if (player->position.y < 0) {
        //La establece en 0
        player->position.y = 0;
    }
    
    //Si la coordenada x del jugador es mayor o igual a el alto total del área de juego
    if (player->position.y >= GAME_HEIGHT) {
        //La establece en el alto total del área de juego decrementado 1 vez
        player->position.y = GAME_HEIGHT - 1;
    }
    
    //-------------------------- MANEJO CARGA DE BOMBA Y DISPAROS DEL JUGADOR --------------------------

    //Si no se ha presionado alguna tecla, o si se ha presionado alguna tecla pero esta no era FIRE (Barra espaciadora)
    if (!(input_mask & INPUT_FIRE)) {
        //Si el número de frames que el jugador ha pasado cargando el arma es menor al máximo establecido en config.h
        if (player->charge_frames < PLAYER_CHARGE_MAX) {
            //Incrementa el contador de frames de carga que han pasado
            player->charge_frames += 1;
        }
        return; //Sale de la función, indicando que no se ha querido dispara el arma gracias a la condición del IF, ya que, para no entrar acá,
                //  SE DEBE haber presionado almenos una tecla Y que esta haya sido la correspondiente a disparo (FIRE (Barra espaciadora))
    }

    //Si la cantidad de frames que el jugador ha pasado cargando el arma es mayor o igual al requerido para accionar la misma
    if (player->charge_frames >= PLAYER_CHARGE_RELEASE_MIN) {
        player->charge_bomb_ready = 1;  //Marca la bomba como lista
        player->charge_frames = 0;  //Reestablece el contador de frames de carga
        player->shot_cooldown = PLAYER_SHOT_COOLDOWN;   //Además, establece el intervalo de frames hasta el siguiente disparo, en el valor definido en config.h
        return;
    }

    //Como se ha disparado el arma, se pierde la carga que el jugador ya tenía, por tanto:
    player->charge_frames = 0;  //Reestablece el contador de frames de carga

    //Si el intervalo de frames hasta el siguiente disparo es igual a cero
    if (player->shot_cooldown == 0) {
        sound_shot(); //Reproduce sonido de disparo
        fire_current_weapon(player, player_shots, shot_count);  //Hace que se dispare el arma actual del jugador
        fire_drones(player, player_shots, shot_count);  //Hace que los drones con los que cuenta el jugador disparen junto a este
        player->shot_cooldown = PLAYER_SHOT_COOLDOWN;   //Además, establece el intervalo de frames hasta el siguiente disparo, en el valor definido en config.h
    }
}
