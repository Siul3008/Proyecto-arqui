#include "powerup.h"

#include "config.h"

/************************************************************************
* Función: 
    inside_board
* Descripción: 
    Verifica si las coordenadas ingresadas se encuentran dentro del 
        área de juego
* Entradas: 
    Vector con coordenadas (x,y)
* Salidas: 
    1 -> Las coordenadas se encuentran dentro del área de juego
    0 -> Las coordenadas no se encuentran dentro del área de juego
*************************************************************************/
static int inside_board(Vec2i position)
{
    /*Compara:
        Si la coordenada x es mayor o igual a cero
        Si la coordenada x es menor a el ancho total del área de juego
        Si la coordenada y es mayor o igual a cero
        Si la coordenada y es menor a la altura total del área de juego
    Si todo lo anterior es verdadero, retornará 1, de lo contrario, 0*/
    return position.x >= 0 && position.x < GAME_WIDTH &&
           position.y >= 0 && position.y < GAME_HEIGHT;
}

/************************************************************************
* Función: 
    powerups_clear
* Descripción: 
    Limpia todos los power-ups presentes en el juego
* Entradas: 
    Arreglo de instancias de objeto PowerUp (power-ups)
    Número entero que indica la cantidad de power-ups activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
void powerups_clear(PowerUp powerups[], int count)
{
    //Mientras i sea menor a la cantidad de maxima de power-ups admitida en el juego
    for (int i = 0; i < count; ++i) {
        //Para el power-up guardado en el índice i actual:
        powerups[i].active = 0; //Lo marca como inactivo
        powerups[i].position.x = 0; //Reestablece su coordenada x
        powerups[i].position.y = 0; //Reestablece su coordenada y
        powerups[i].type = POWERUP_WEAPON;  //Reestablece su tipo de power-up
        powerups[i].weapon = WEAPON_FRONT;  //Reestablece su tipo de arma
    }
}

/************************************************************************
* Función: 
    powerups_spawn
* Descripción: 
    Genera la cantidad de power-ups que se requiera
* Entradas: 
    Arreglo de instancias de objeto PowerUp (power-ups)
    Número entero que indica la cantidad de power-ups activos en el juego
    Vector con coordenadas (x,y) desde las cuales se desea se genere el 
        power-up
    Tipo de power-up deseado tipo PowerUpType
    Tipo de arma deseada tipo WeaponType
* Salidas: 
    Ninguna
*************************************************************************/
static void powerups_spawn(PowerUp powerups[], int count, Vec2i position, PowerUpType type, WeaponType weapon)
{
    //Mientras i sea menor a la cantidad de maxima de power-ups admitida en el juego
    for (int i = 0; i < count; ++i) {
        //Si el power-up en el índice actual se encuentra inactivo
        if (!powerups[i].active) {
            //Para el power-up guardado en el índice i actual:
            powerups[i].active = 1; //Lo marca como activo
            powerups[i].position = position;    //Establece su coordenada de aparición de acuerdo a la ingresada
            powerups[i].type = type;    //Establece su tipo de power-up de acuerdo al ingresado
            powerups[i].weapon = weapon;    //Establece su tipo de arma de acuerdo al ingresado
            return; //Sale de la función
        }
    }
}

/************************************************************************
* Función: 
    powerups_spawn_weapon
* Descripción: 
    Genera un power-up que será un arma
* Entradas: 
    Arreglo de instancias de objeto PowerUp (power-ups)
    Número entero que indica la cantidad de power-ups activos en el juego
    Vector con coordenadas (x,y) desde las cuales se desea se genere el 
        power-up
    Tipo de arma deseada tipo WeaponType
* Salidas: 
    Ninguna
*************************************************************************/
void powerups_spawn_weapon(PowerUp powerups[], int count, Vec2i position, WeaponType weapon)
{
    //Genera un power-up tipo arma de acuerdo a los parámetros ingresados
    powerups_spawn(powerups, count, position, POWERUP_WEAPON, weapon);
}

/************************************************************************
* Función: 
    powerups_spawn_drone
* Descripción: 
    Genera un power-up que será un dron
* Entradas: 
    Arreglo de instancias de objeto PowerUp (power-ups)
    Número entero que indica la cantidad de power-ups activos en el juego
    Vector con coordenadas (x,y) desde las cuales se desea se genere el 
        power-up
* Salidas: 
    Ninguna
*************************************************************************/
void powerups_spawn_drone(PowerUp powerups[], int count, Vec2i position)
{
    //Genera un power-up tipo dron de acuerdo a los parámetros ingresados
    powerups_spawn(powerups, count, position, POWERUP_DRONE, WEAPON_FRONT);
}

/************************************************************************
* Función: 
    powerups_update
* Descripción: 
    Actualiza el estado de los powerups activos en el juego
* Entradas: 
    Arreglo de instancias de objeto PowerUp (power-ups)
    Número entero que indica la cantidad de power-ups activos en el juego
    Número entero que indica el número de frame actual del juego
    Número entero que indica el intervalo (en frames) entre cada 
        movimiento
* Salidas: 
    Ninguna
*************************************************************************/
void powerups_update(PowerUp powerups[], int count, int frame, int move_interval)
{   
    //Si el intervalo de movimiento es igual o menor a cero, o si el residuo de dividir
    //  el frame actual entre el intervalo de movimiento no es iguaol a cero
    if (move_interval <= 0 || frame % move_interval != 0) {
        return; //Sale de la función
    }

    //Mientras i sea menor a la cantidad de maxima de power-ups admitida en el juego
    for (int i = 0; i < count; ++i) {
        //Si el power-up en el índice actual está inactivo
        if (!powerups[i].active) {
            //Pasa a la siguiente iteración, es decir, pasa procesar el siguiente powerup guardado en el arreglo
            continue;
        }

        //Si este se encuentra activo, incrementa una vez su coordenada y, haciendo que vaya hacia abajo
        powerups[i].position.y += 1;

        //Si el powerup actual ya no se encuentra dentro del área de juego
        if (!inside_board(powerups[i].position)) {
            powerups[i].active = 0; //Procede a marcarlo como inactivo
        }
    }
}

/************************************************************************
* Función: 
    powerups_weapon_for_seed
* Descripción: 
    Determina el arma de power-up que se obtendrá de acuerdo a la 
        semilla de generación que se proporcione
* Entradas: 
    Número entero correspondiente a la semilla de generación
* Salidas: 
    Tipo de arma WeaponType
*************************************************************************/
WeaponType powerup_weapon_for_seed(int seed)
{
    //Toma el residuo de divir la semilla de generación entre 8
    switch (seed % 8) {
    //Si este residuo es igual a 0, 1 o 2
    case 0:
    case 1:
    case 2:
        //Retorna un arma tipo SPREAD
        return WEAPON_SPREAD;
    //Si es igual a 3 o 4
    case 3:
    case 4:
        //Retorna un arma tipo DOUBLE
        return WEAPON_DOUBLE;
    //Si es igual a 5
    case 5:
        //Retorna un arma tipo LASER
        return WEAPON_LASER;
    //De lo contrario
    default:
        //Retorna un arma tipo SIDE
        return WEAPON_SIDE;
    }
}

/************************************************************************
* Función: 
    powerups_type_for_seed
* Descripción: 
    Determina el tipo de power-up que se obtendrá de acuerdo a la 
        semilla de generación que se proporcione
* Entradas: 
    Número entero correspondiente a la semilla de generación
* Salidas: 
    Tipo de power-up PowerUpType
*************************************************************************/
PowerUpType powerup_type_for_seed(int seed)
{
    //Toma la el residuo de dividir la semilla entre 7
    //  Si este es igual a 0 -> Retorna tipo dron
    //  De lo contrario -> Retorna tipo arma
    return seed % 7 == 0 ? POWERUP_DRONE : POWERUP_WEAPON;
}
