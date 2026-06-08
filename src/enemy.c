#include "enemy.h"

#include "config.h"
#include "projectile.h"

/************************************************************************
* Función: 
    enemy_is_boss_type
* Descripción: 
    Verifica si el enemigo actual es un jefe
* Entradas: 
    Tipo de enemigo EnemyType
* Salidas: 
    1 => El enemigo es un jefe
    0 => El enemigo no es un jefe
*************************************************************************/
static int enemy_is_boss_type(EnemyType type)
{
    return type == ENEMY_MINI_BOSS || type == ENEMY_STAGE_BOSS;
}

/************************************************************************
* Función: 
   move_boss_horizontal
* Descripción: 
    Mueve al jefe de derecha a izquierda y visceversa en la pantalla, 
    rebotando una vez alcanza el límite, sea izquierdo o derecho, del 
    área de juego
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Ninguna
*************************************************************************/
static void move_boss_horizontal(Enemy *enemy)
{
    //Calcula la próxima coordenada x del jefe al tomar la coordenada actual, 
    //  y sumarle el valor de velocidad con el que este cuenta
    int next_x = enemy->position.x + enemy->velocity.x;
    //Obtiene cuales son los límtes izquierdo y derecho
    int left_limit = enemy_min_center_x(enemy->type); 
    int right_limit = enemy_max_center_x(enemy->type);

    //Si la velocidad del enemigo es cero
    if (enemy->velocity.x == 0) {
        enemy->velocity.x = 1; //La establece en 1 (movimiento a la derecha)
        next_x = enemy->position.x + enemy->velocity.x; //Y actualiza la próxima coordenada x
    }

    //Si la próxima coordenada x es menor o igual al límite izquierdo
    if (next_x <= left_limit) {
        enemy->position.x = left_limit; //Actualiza la coordenada actual a ese límite
        enemy->velocity.x = 1;  //Establece la velocidad en 1 (movimiento a la derecha)

    //Si la próxima coordenada x es mayor o igual al límite derecho    
    } else if (next_x >= right_limit) {
        enemy->position.x = right_limit; //Actualiza la coordenada actual a ese límite
        enemy->velocity.x = -1; /*Establece la velocidad en -1 (movimiento a la izquierda)

                                    (Se usa un valor negativo ya que hay que recordar que 
                                    la velocidad es un vector con dirección, donde el negativo
                                    indica que va en dirección contraria a la dirección original)*/
    //De lo contrario
    } else {
        enemy->position.x = next_x; //Solo actualiza la coordenada x actual
    }
}

/************************************************************************
* Función: 
   move_enemy
* Descripción: 
    Mueve al enemigo tomando en cuenta el tipo de enemigo asignado a de 
    manera aleatoria en el momento de su creación
    (Si no es tipo BOSS (jefe), su tipo determinará el patrón de movimiento
    que debe seguir)
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Ninguna
*************************************************************************/
static void move_enemy(Enemy *enemy)
{
    switch (enemy->type) {
    //Si el enemigo es tipo STRAIGHT
    case ENEMY_STRAIGHT:
        enemy->position.y += enemy->velocity.y; //Solo hace que el enemigo se mueva en línea recta hacia abajo
        break;
    //Si el enemigo es tipo diagonal    
    case ENEMY_DIAGONAL:
        enemy->position.x += enemy->velocity.x; //Le suma la velocidad correspondiente tanto a la coordenada x como y, haciendo que se mueva de forma diagonal
        enemy->position.y += enemy->velocity.y;
        break;
    //Si el enemigo es tipo ZIGZAG    
    case ENEMY_ZIGZAG:
        //Si el tiempo que el enemigo lleva activo es divisible entre diez
        if (enemy->age % 10 == 0) {
            enemy->velocity.x *= -1; //Establece la velocidad en -1 (movimiento a la izquierda)
        }
        //Le suma la velocidad correspondiente tanto a la coordenada x como y, haciendo que, dependiendo de la velocidad horizontal, se mueva hacia abajo y hacia la izquierda, o derecha
        enemy->position.x += enemy->velocity.x; 
        enemy->position.y += enemy->velocity.y;
        break;
    //Si el enemigo es tipo FAST
    case ENEMY_FAST:
        //Le suma la velocidad correspondiente tanto a la coordenada x como y, haciendo que se mueva de forma diagonal
        // tomando en cuenta que su velocidad vertical será mayor al haber incrementado la velocidad vertical original 
        enemy->position.x += enemy->velocity.x;
        enemy->position.y += enemy->velocity.y + 1;
        break;
    case ENEMY_SWEEP:
        enemy->position.x += enemy->velocity.x;
        if (enemy->age % 2 == 0) {
            enemy->position.y += enemy->velocity.y;
        }
        break;
    case ENEMY_DIVE:
        if (enemy->age < 20) {
            enemy->position.x += enemy->velocity.x;
            enemy->position.y += enemy->velocity.y;
        } else {
            enemy->position.y += enemy->velocity.y + 1;
        }
        break;
    //Si el enemigo es un tipo de jefe
    case ENEMY_MINI_BOSS:
    case ENEMY_STAGE_BOSS:
        //Pasa a la función dedicada para el movimiento de los jefes
        move_boss_horizontal(enemy);
        break;
    }
}

/************************************************************************
* Función: 
   keep_boss_inside_vertical_bounds
* Descripción: 
    Se asegura que el jefe, al aparecer, se encuentre dentro de los
    límites verticales del área de juego
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Ninguna
*************************************************************************/
static void keep_boss_inside_vertical_bounds(Enemy *enemy)
{
    //Verifica que el enemigo realmente sea un jefe
    if (!enemy_is_boss_type(enemy->type)) {
        //Si no lo es, sale dela función
        return;
    }

    //Si su coordenada y se encuentra fuera del área de juego
    if (enemy->position.y < 0 || enemy->position.y >= GAME_HEIGHT) {
        enemy->position.y = 2; //Establece la coordenada y en 2
    }

    enemy->velocity.y = 0; //Además, establece su velocidad vertical en 0
}

/************************************************************************
* Función: 
   keep_enemy_inside_horizontal_bounds
* Descripción: 
    Se asegura que el enemigo se mantenga dentro del área de juego
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Ninguna
*************************************************************************/
static void keep_enemy_inside_horizontal_bounds(Enemy *enemy)
{
    //Obtiene cuales son los límtes izquierdo y derecho
    int left_limit = enemy_min_center_x(enemy->type);
    int right_limit = enemy_max_center_x(enemy->type);

    //Si la coordenada x del enemigo es menor al límte izquierdo o el lado izquierdo de su hitbox es menor a cero
    if (enemy->position.x < left_limit || enemy_left(enemy) < 0) {
        enemy->position.x = left_limit; //Establece su coordenada x en el límite izquierdo
        enemy->velocity.x = 1;  //Establece su velocidad en 1
    //Si la coordenada x del enemigo es menor al límte izquierdo o el lado izquierdo de su hitbox es menor a cero
    } else if (enemy->position.x > right_limit || enemy_right(enemy) > GAME_WIDTH - 1) {
        enemy->position.x = right_limit; //Establece su coordenada x en el límite izquierdo
        enemy->velocity.x = -1; //Establece su velocidad en 1
    }
}

/************************************************************************
* Función: 
   fire_cooldown_for_type
* Descripción: 
    Según el tipo de enemigo, le asigna un valor para el tiempo que debe 
    haber entre disparos
* Entradas:
    Tipo de enemigo EnemyType
    Número entero (parámetro llamado slot ya que, en funciones donde se 
    llama a esta función, ese entero va a ser el índice del arreglo que 
    contiene a los enemigos)
* Salidas: 
    Tiempo que debe haber entre disparos
*************************************************************************/
static int fire_cooldown_for_type(EnemyType type, int slot)
{
    switch (type) {
    case ENEMY_STRAIGHT:
        return 18 + (slot % 4);
    case ENEMY_DIAGONAL:
        return 15 + (slot % 5);
    case ENEMY_ZIGZAG:
        return 13 + (slot % 4);
    case ENEMY_FAST:
        return 18 + (slot % 6);
    case ENEMY_SWEEP:
        return 16 + (slot % 5);
    case ENEMY_DIVE:
        return 20 + (slot % 4);
    case ENEMY_MINI_BOSS:
        return 12;
    case ENEMY_STAGE_BOSS:
        return 9;
    }

    return 18;
}

/************************************************************************
* Función: 
   fire_mini_boss_pattern
* Descripción: 
    Genera el patrón de disparos del minijefe según los parámetros 
    ingresados
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
    Arreglo de instancias de objeto Projectile (proyectiles)
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_mini_boss_pattern(const Enemy *enemy, Projectile enemy_shots[], int shot_count)
{
    //Calcula la posición central y las laterales desde donde van a salir los proyectiles
    Vec2i center = {enemy->position.x, enemy->position.y + 1};
    Vec2i left = {enemy->position.x - 1, enemy->position.y + 1};
    Vec2i right = {enemy->position.x + 1, enemy->position.y + 1};

    //Si el resultado de dividir el tiempo que lleva activo el enemigo entre 40, es divisible entre 2
    if ((enemy->age / 40) % 2 == 0) {
        /*Pasa los siguientes parámetros a la función projectiles_spawn (Una vez por punto de origen):
            Arreglo de instancias de objeto Projectile (proyectiles)
           Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que los proyectiles vayan directamente hacia abajo, se establece 
                la velocidad x en 0 y la velocidad y en 1, para que así solo se mueva de manera vertical hacia
                abajo, mas no horizontal)  */
        projectiles_spawn(enemy_shots, shot_count, left, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, right, (Vec2i){0, 1});
    } else {
        /*Pasa los siguientes parámetros a la función projectiles_spawn (Una vez por punto de origen):
            Arreglo de instancias de objeto Projectile (proyectiles)
           Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo (En este caso, se desea que todos aparezcan desde el centro)
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que los proyectiles se dispersen, se colocan los siguientes parámetros en dicho vector:
                    (0,1) -> Directamente hacia abajo
                    (-1, 1) -> Diagonal hacia abajo y a la izquierda
                    (1, 1) -> Diagonal hacia abajo y a la derecha)*/
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){-1, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){1, 1});
    }
}

/************************************************************************
* Función: 
   fire_stage_boss_pattern
* Descripción: 
    Genera el patrón de disparos del jefe según los parámetros ingresados
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
    Arreglo de instancias de objeto Projectile (proyectiles)
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
static void fire_stage_boss_pattern(const Enemy *enemy, Projectile enemy_shots[], int shot_count)
{   
    //Calcula la posición central, laterales cercanas y laterales lejanas desde donde van a salir los proyectiles
    Vec2i center = {enemy->position.x, enemy->position.y + 1};
    Vec2i far_left = {enemy->position.x - 2, enemy->position.y + 1};
    Vec2i left = {enemy->position.x - 1, enemy->position.y + 1};
    Vec2i right = {enemy->position.x + 1, enemy->position.y + 1};
    Vec2i far_right = {enemy->position.x + 2, enemy->position.y + 1};

    //Calcula el patrón a generar, esto al tomar el residuo de dividir entre 3 
    //  el resultado de la división del tiempo que lleva activo el enemigo entre 50
    int pattern = (enemy->age / 50) % 3;

    //Si este residuo es igual a cero
    if (pattern == 0) {
        /*Pasa los siguientes parámetros a la función projectiles_spawn (Una vez por punto de origen, que en este 
        como se desea que hayan 5 proyectiles activos, se usan los 5 puntos de origen calculados previamente):
            Arreglo de instancias de objeto Projectile (proyectiles)
           Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que solo los proyectiles que salen desde los extremos lejanos vayan de 
                forma diagonal, se colocan los siguientes parámeros en dicho vector:
                    (0,1) -> Directamente hacia abajo (Centro y costados cercanos)
                    (-1, 1) -> Diagonal hacia abajo y a la izquierda (Costado izquierdo lejano)
                    (1, 1) -> Diagonal hacia abajo y a la derecha (Costado derecho lejano))  */
        projectiles_spawn(enemy_shots, shot_count, far_left, (Vec2i){-1, 1});
        projectiles_spawn(enemy_shots, shot_count, left, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, right, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, far_right, (Vec2i){1, 1});
    //Si este residuo es igual a 1
    } else if (pattern == 1) {
        /*Pasa los siguientes parámetros a la función projectiles_spawn (Una vez por punto de origen, que en este 
        como se desea que hayan 3 proyectiles activos, se usan los 3 puntos de origen "cercanos" calculados previamente):
            Arreglo de instancias de objeto Projectile (proyectiles)
            Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que solo los proyectiles que salen desde los extremos cercanos vayan de 
                forma diagonal, se colocan los siguientes parámeros en dicho vector:
                    (0,1) -> Directamente hacia abajo (Centro)
                    (-1, 1) -> Diagonal hacia abajo y a la izquierda (Costado izquierdo cercano)
                    (1, 1) -> Diagonal hacia abajo y a la derecha (Costado derecho cercano))  */
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){-1, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, center, (Vec2i){1, 1});
    //Si este residuo no es igual ni a cero ni a uno
    } else {
        /*Pasa los siguientes parámetros a la función projectiles_spawn (Una vez por punto de origen, que en este 
        como se desea que hayan 2 proyectiles activos, se usan los 2 puntos de "lejanos" calculados previamente):
            Arreglo de instancias de objeto Projectile (proyectiles)
           Número entero que indica la cantidad de proyectiles activos en el juego
            Vector con las coordenadas donde se desea que se genere el disparo
            Vector con las velocidades "x" y "y" correspondientes 
                (En este caso, como se quiere que ambos proyectiles vayan directamente hacia abajo, se colocan los
                siguientes parámeros en dicho vector:
                    (0,1) -> Directamente hacia abajo  */
        projectiles_spawn(enemy_shots, shot_count, far_left, (Vec2i){0, 1});
        projectiles_spawn(enemy_shots, shot_count, far_right, (Vec2i){0, 1});
    }
}

/************************************************************************
* Función: 
    enemy_max_health
* Descripción: 
    Retorna cual es la vida máxima del enemigo según su tipo
* Entradas:
    Tipo de enemigo EnemyType
* Salidas: 
    Entero con la cantidad de vida que tiene el enemigo
*************************************************************************/
int enemy_max_health(EnemyType type)
{
    switch (type) {
    case ENEMY_MINI_BOSS:
        return MINI_BOSS_HEALTH;
    case ENEMY_STAGE_BOSS:
        return STAGE_BOSS_HEALTH;
    default:
        return 1;
    }
}

/************************************************************************
* Función: 
    enemy_hitbox_half_width
* Descripción: 
    Retorna cual es la coordenada x en la que se encuentra el centro del 
    hitbox del enemigo según su tipo
* Entradas:
    Tipo de enemigo EnemyType
* Salidas: 
    Entero con coordenada x en la que se encuentra el centro del hitbox
*************************************************************************/
int enemy_hitbox_half_width(EnemyType type)
{
    switch (type) {
    case ENEMY_MINI_BOSS:
        return 2;
    case ENEMY_STAGE_BOSS:
        return 3;
    default:
        return 0;
    }
}

/************************************************************************
* Función: 
    enemy_min_center_x
* Descripción: 
    Retorna cual es la coordenada central x mínima que puede tener un 
    enemigo según su tipo
* Entradas:
    Tipo de enemigo EnemyType
* Salidas: 
    Entero con la coordenada central x mínima del enemigo
*************************************************************************/
int enemy_min_center_x(EnemyType type)
{
    //Retorna del valor de la mitad de su hitbox
    return enemy_hitbox_half_width(type);
}

/************************************************************************
* Función: 
    enemy_max_center_x
* Descripción: 
    Retorna cual es la coordenada central x máxima que puede tener un 
    enemigo según su tipo
* Entradas:
    Tipo de enemigo EnemyType
* Salidas: 
    Entero con la coordenada central x máxima del enemigo
*************************************************************************/
int enemy_max_center_x(EnemyType type)
{
    //Toma el ancho total del área de juego, le resta 1, y le resta la mitad 
    //  del tamaño de la hitbox del enemigo, para así obtener su coordenada x 
    // central máxima
    return GAME_WIDTH - 1 - enemy_hitbox_half_width(type);
}

/************************************************************************
* Función: 
    enemy_left
* Descripción: 
    Retorna cual es la coordenada x del lado izquierdo de la hitbox del
    enemigo
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Entero con la coordenada x del lado izquierdo de la hitbox del
    enemigo
*************************************************************************/
int enemy_left(const Enemy *enemy)
{
    //Toma la coordenada central x del enemigo y le resta la mitad 
    //  del tamaño de su hitbox
    return enemy->position.x - enemy_hitbox_half_width(enemy->type);
}

/************************************************************************
* Función: 
    enemy_right
* Descripción: 
    Retorna cual es la coordenada x del lado derecho de la hitbox del
    enemigo
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Entero con la coordenada x del lado derecho de la hitbox del
    enemigo
*************************************************************************/
int enemy_right(const Enemy *enemy)
{
    //Toma la coordenada central x del enemigo y le suma la mitad 
    //  del tamaño de su hitbox
    return enemy->position.x + enemy_hitbox_half_width(enemy->type);
}

/************************************************************************
* Función: 
    enemy_top
* Descripción: 
    Retorna cual es la coordenada y del lado superior de la hitbox del
    enemigo
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Entero con la coordenada y del lado superior de la hitbox del
    enemigo
*************************************************************************/
int enemy_top(const Enemy *enemy)
{
    //Ya que los enemigos solo tienen un caracter de altura, directamente retorna 
    //  la coordenada y de este
    return enemy->position.y;
}

/************************************************************************
* Función: 
    enemy_top
* Descripción: 
    Retorna cual es la coordenada y del lado inferior de la hitbox del
    enemigo
* Entradas:
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Entero con la coordenada y del lado inferior de la hitbox del
    enemigo
*************************************************************************/
int enemy_bottom(const Enemy *enemy)
{
    //Ya que los enemigos solo tienen un caracter de altura, directamente retorna 
    //  la coordenada y de este
    return enemy->position.y;
}

/************************************************************************
* Función: 
   enemies_clear
* Descripción: 
    Le hace un reset a todos los enemigos del juego, efectivamente 
    eliminandolos
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos a desactivar
* Salidas: 
    Ninguna
*************************************************************************/
void enemies_clear(Enemy enemies[], int count)
{
    //Mientras i sea menor a la cantidad de enemigos a desactivar indicada
    for (int i = 0; i < count; ++i) {
        enemies[i].active = 0; //Lo marca como inactivo 
        enemies[i].position.x = 0;  //Reestablece sus coordenadas (x,y) en (0,0)
        enemies[i].position.y = 0;
        enemies[i].velocity.x = 0;  //Establece su velocidad horizontal en 0
        enemies[i].velocity.y = 1;  //Establece su velocidad vertical en 1
        enemies[i].health = 1;      //Establece cu cantidad de vidas en 1
        enemies[i].fire_cooldown = 0;   //Establece el tiempo entre disparos en 0
        enemies[i].age = 0;     //Establece su tiempo de vida en 0
        enemies[i].type = ENEMY_STRAIGHT;  //Establece su tipo de enemigo como STRAIGHT
    }
}

/************************************************************************
* Función: 
   enemies_spawn
* Descripción: 
    Spawnea los enemigos en el juego 
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos activos que se quiere
    Entero que indica la posición x deseada para los enemigos
    Tipo de enemigo EnemyType
* Salidas: 
    Ninguna
*************************************************************************/
void enemies_spawn(Enemy enemies[], int count, int x, EnemyType type)
{
    //Mientras i sea menor a la cantidad de enemigos a generar indicada
    for (int i = 0; i < count; ++i) {
        //Si el enemigo en el indice actual se encuentra inactivo
        if (!enemies[i].active) {
            enemies[i].active = 1;  //Lo marca como activo
            enemies[i].position.x = x;  //Establece su coordenada x en la x indicada
            enemies[i].position.y = 0;  //Establece su coordenada y en 0

            /*Toma el residuo de dividir entre dos el resultado de la suma de la coordenada x indicada y el 
            índice actual:
            Si este es igual a cero -> Velocidad horizontal = 1 (Se mueve hacia la derecha)
            Si este no es igual a cero -> Velocidad horizontal = -1 (Se mueve hacia la izquierda) */
            enemies[i].velocity.x = ((x + i) % 2 == 0) ? 1 : -1; 
            enemies[i].velocity.y = 1; //Establece su velocidad vertical en 1 (Se mueve hacia abajo)
            enemies[i].health = enemy_max_health(type); //Establece su cantidad de vidas según su tipo

             //Establece el tiempo entre disparos según su tipo y el índice de lista actual
            enemies[i].fire_cooldown = fire_cooldown_for_type(type, i);
            enemies[i].age = 0; //Establece su tiempo de vida en 0
            enemies[i].type = type; //Establece su tipo en el tipo indicado
            return;
        }
    }
}

/************************************************************************
* Función: 
   enemies_spawn
* Descripción: 
    Spawnea los jefes en el juego 
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos activos que se quiere
    Tipo de enemigo EnemyType
* Salidas: 
    Ninguna
*************************************************************************/
void enemies_spawn_boss(Enemy enemies[], int count, EnemyType type)
{
    //Mientras i sea menor a la cantidad de jefes a generar indicada
    for (int i = 0; i < count; ++i) {
        //Si el enemigo en el indice actual se encuentra inactivo
        if (!enemies[i].active) {
            enemies[i].active = 1;  //Lo marca como activo

            //Establece su coordenada x en el centro del área de juego
            enemies[i].position.x = GAME_WIDTH / 2; 
            enemies[i].position.y = 2;  //Establece su coordenada y en 2
            enemies[i].velocity.x = 1;  //Establece su velocidad horizontal en 1 (Se mueve hacia la derecha)
            enemies[i].velocity.y = 0;  //Establece su velocidad vertical en 0 (No sube ni baja)
            enemies[i].health = enemy_max_health(type); //Establece su cantidad de vidas según su tipo

            //Establece el tiempo entre disparos según su tipo y el índice de lista actual
            enemies[i].fire_cooldown = fire_cooldown_for_type(type, i);
            enemies[i].age = 0; //Establece su tiempo de vida en 0
            enemies[i].type = type; //Establece su tipo en el tipo indicado
            return;
        }
    }
}

/************************************************************************
* Función: 
   enemies_update
* Descripción: 
    Actualiza el estado de todos los enemigos en el juego
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos en el juego
    Arreglo de instancias de objeto Projectile (proyectiles) (En este
        caso de los enemigos)
    Número entero que indica la cantidad de proyectiles activos en el 
        juego
    Número entero que indica el número de frame actual del juego
    Número entero que indica el número de frames que deben pasar para 
        que los enemigos se puedan mover, es decir, su intervalo de 
        movimiento
* Salidas: 
    Ninguna
*************************************************************************/
void enemies_update(Enemy enemies[], int enemy_count, Projectile enemy_shots[], int shot_count, int frame, int move_interval)
{
    //Mientras i sea menor a la cantidad enemigos en el juego
    for (int i = 0; i < enemy_count; ++i) {
        //Asigna a un puntero Enemy un enemigo del arreglo poporcionado por medio de referencia
        Enemy *enemy = &enemies[i];

        //Si el enemigo se encuntra inactivo, inmediatamente pasa al siguiente índice, es decir, al siguiente enemigo del arreglo
        if (!enemy->active) {
            continue;
        }

        //Si el intervalo de movimiento es menor o igual a cero, o si el residuo de la division del número de frame actual y 
        //  intervalo de movimiento indicado es igual a cero
        if (move_interval <= 0 || frame % move_interval == 0) {
            //Mueve el enemigo actual
            move_enemy(enemy);
        }
        //Le añade un frame de antiguedad al tiempo de vida del enemigo
        enemy->age += 1;

        //Se asegura que el enemigo se mantenga dentro de los límites laterales del área de juego sin importar si es jefe o no
        keep_enemy_inside_horizontal_bounds(enemy);
        //Si es jefe, se asegura que este se encuentra dentro de los límites verticales del área de juiego
        keep_boss_inside_vertical_bounds(enemy); //(Dentro de la función verifica si el enemigo es jefe o no)

        //Si la coordenada y es mayor o igual a la altura máxima del área de juego y además el enemigo actual no es un jefe
        if (enemy->position.y >= GAME_HEIGHT &&
            !enemy_is_boss_type(enemy->type)) {
            //Marca el enemigo como inactivo (Ya que salió del área de juego) y pasa al siguiente índice, es decir, al siguiente 
            // enemigo del arreglo
            enemy->active = 0;
            continue;
        }

        //Si el intervalo de frames entre disparos del enemigo es mayor a cero
        if (enemy->fire_cooldown > 0) {
            //Le resta 1 a este intervalo
            enemy->fire_cooldown -= 1;
        } else {
            //De lo contrario, si el enemigo es un jefe
            if (enemy->type == ENEMY_STAGE_BOSS) {
                /*Llama a función que genere el patrón de disparos correpondiente para un jefe, pasando los siguientes parámetros
                    Puntero a instancia de clase Enemy (Enemigo)
                    Arreglo de instancias de objeto Projectile (proyectiles) (En este
                        caso de los enemigos)
                    Número entero que indica la cantidad de proyectiles activos en el 
                    juego*/
                fire_stage_boss_pattern(enemy, enemy_shots, shot_count);

            //Por otro lado, si el enemigo es un mini jefe
            } else if (enemy->type == ENEMY_MINI_BOSS) {
                /*Llama a función que genere el patrón de disparos correpondiente para un mini jefe, pasando los siguientes parámetros
                    Puntero a instancia de clase Enemy (Enemigo)
                    Arreglo de instancias de objeto Projectile (proyectiles) (En este
                        caso de los enemigos)
                    Número entero que indica la cantidad de proyectiles activos en el 
                    juego*/
                fire_mini_boss_pattern(enemy, enemy_shots, shot_count);

            //De lo contrario
            } else {
                //Calcula las coordenadas iniciales del disparo basándose en las coordenadas del enemigo, asegurandose que este
                //  proyectil sea generado justo debajo del enemigo
                Vec2i shot_position = {enemy->position.x, enemy->position.y + 1};

                //Establece un vector con las velocidades x y y correspondientemente:
                //  velocidad x = 0 (No hay movimiento horizontal)
                //  velocidad y = 1 (Se mueve hacia abajo)
                Vec2i shot_velocity = {0, 1};
                /*Llama a función que genere los proyectiles solicitados, pasando los siguientes parámetros
                    Puntero a instancia de clase Enemy (Enemigo)
                    Arreglo de instancias de objeto Projectile (proyectiles) (En este
                        caso de los enemigos)
                    Número entero que indica la cantidad de proyectiles activos en el 
                    juego
                    Vector con coordenadas de origen del proyectil
                    Vector con velocidades que tendrá el proyectil*/
                projectiles_spawn(enemy_shots, shot_count, shot_position, shot_velocity);
            }
            enemy->fire_cooldown = fire_cooldown_for_type(enemy->type, i);
        }
    }
}

/************************************************************************
* Función: 
   enemies_find_boss
* Descripción: 
    Encuentra un jefe dentro del arreglo de enemigos del juego
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos en el juego
* Salidas: 
    Puntero a objeto Enemy (enemigo), que en este caso, será un jefe
    0 -> No se ha encontrado ningún jefe en el arreglo proporcionado
*************************************************************************/
const Enemy *enemies_find_boss(const Enemy enemies[], int count)
{
    //Mientras i sea menor a la cantidad enemigos en el juego
    for (int i = 0; i < count; ++i) {
        //Si el enemigo actual se encuentra activo y es tipo jefe
        if (enemies[i].active && enemy_is_boss_type(enemies[i].type)) {
            //Retorna por medio de referencia, el puntero al enemigo tipo jefe
            return &enemies[i];
        }
    }
    //Si no ha encontrado ningún jefe, retorna 0
    return 0;
}

/************************************************************************
* Función: 
   enemies_has_boss
* Descripción: 
    Indica si hay al menos un jefe dentro del arreglo de enemigos del 
        juego
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos en el juego
* Salidas: 
    1 -> Se ha encontrado al menos un jefe en el arreglo proporcionado
    0 -> No se ha encontrado ningún jefe en el arreglo proporcionado
*************************************************************************/
int enemies_has_boss(const Enemy enemies[], int count)
{
    //Mientras i sea menor a la cantidad enemigos en el juego
    for (int i = 0; i < count; ++i) {
        //Si el enemigo actual se encuentra activo y es tipo jefe
        if (enemies[i].active && enemy_is_boss_type(enemies[i].type)) {
            //Retorna 1
            return 1;
        }
    }
    //Si no ha encontrado ningún jefe, retorna 0    
    return 0;
}

/************************************************************************
* Función: 
   enemies_has_boss
* Descripción: 
    Indica la cantidad de enemigos activos actualmente en el juego
* Entradas:
    Arreglo de instancias de objeto Enemy (enemigos)
    Número entero que indica la cantidad de enemigos en el juego
* Salidas: 
    Cantidad de enemigos activos
*************************************************************************/
int enemies_active_count(const Enemy enemies[], int count)
{
    //Inicializa la cuenta en cero
    int active_count = 0;

    //Mientras i sea menor a la cantidad enemigos en el juego
    for (int i = 0; i < count; ++i) {
        //Si el enemigo actual se encuentra activo
        if (enemies[i].active) {
            //Incrementa en 1 la cuenta
            active_count += 1;
        }
    }
    //Retorna la cuenta de enemigos activos
    return active_count;
}

/************************************************************************
* Función: 
   enemy_type_for_wave
* Descripción: 
    Indica el tipo de enemigo requerido según la ola de enemigos (nivel) 
        en la que se encuentra el juego
* Entradas:
    Número entero que indica el número de ola (nivel) en la que se 
        encuentra el juego
    Número entero que indica el índice de ola generada
* Salidas: 
    Tipo de enemigo EnemyType
*************************************************************************/
EnemyType enemy_type_for_wave(int wave, int spawn_index)
{
    //Si el número de ola es menor o igual a uno (Primera ola)
    if (wave <= 1) {
        //Retorna que el enemigo a usar es tipo STRAIGHT
        return ENEMY_STRAIGHT;
    }
    //Si el número de ola es igual a dos (Segunda ola)
    if (wave == 2) {
        /*Toma el residuo de dividir entre dos el índice de ola generada
            Si este es igual a cero -> El enemigo a usar es tipo STRAIGHT
            Si este no es igual a cero -> El enemigo a usar es tipo DIAGONAL
        Retornará el tipo de enemigo calculado*/
        return (spawn_index % 2 == 0) ? ENEMY_STRAIGHT : ENEMY_DIAGONAL;
    }

    //Si el número de ola es igual a tres (Tercera ola)
    if (wave == 3) {
        /*Toma el residuo de dividir entre 3 el índice de ola generada
            Si este es igual a cero -> El enemigo a usar es tipo ZIGZAG
            Si este no es igual a cero -> El enemigo a usar es tipo DIAGONAL
        Retornará el tipo de enemigo calculado*/
        return (spawn_index % 3 == 0) ? ENEMY_ZIGZAG : ENEMY_DIAGONAL;
    }

    //Si el número de ola es igual a cuatro (Cuarta ola)
    if (wave == 4) {
        /*Toma el residuo de dividir entre 2 el índice de ola generada
            Si este es igual a cero -> El enemigo a usar es tipo SWEEP
            Si este no es igual a cero -> El enemigo a usar es tipo ZIGZAG
        Retornará el tipo de enemigo calculado*/
        return (spawn_index % 2 == 0) ? ENEMY_SWEEP : ENEMY_ZIGZAG;
    }

    //Si el número de ola es igual a cinco (Quinta ola)
    if (wave == 5) {
        /*Toma el residuo de dividir entre 2 el índice de ola generada
            Si este es igual a cero -> El enemigo a usar es tipo DIVE
            Si este no es igual a cero -> El enemigo a usar es tipo FAST
        Retornará el tipo de enemigo calculado*/
        return (spawn_index % 2 == 0) ? ENEMY_DIVE : ENEMY_FAST;
    }

    //Toma el residuo de dividir entre 5 el índice de ola generada
    switch (spawn_index % 6) {
    //Si este es igual a cero
    case 0:
        //Retorna que el enemigo a usar es tipo FAST
        return ENEMY_FAST;
    //Si este es igual a uno
    case 1:
        //Retorna que el enemigo a usar es tipo ZIGZAG
        return ENEMY_ZIGZAG;
    //Si este es igual a dos
    case 2:
        //Retorna que el enemigo a usar es tipo DIAGONAL
        return ENEMY_DIAGONAL;
    //Si este es igual a tres        
    case 3:
        //Retorna que el enemigo a usar es tipo SWEEP
        return ENEMY_SWEEP;
    //Si este es igual a cuatro
    case 4:
        //Retorna que el enemigo a usar es tipo DIVE 
        return ENEMY_DIVE;
    //Si no es ninguno de estos casos
    default:
        //Retorna que el enemigo a usar es tipo STRAIGHT
        return ENEMY_STRAIGHT;
    }
}
