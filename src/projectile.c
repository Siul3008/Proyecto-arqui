#include "projectile.h"

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
    projectiles_clear
* Descripción: 
    Limpia todos los proyectiles presentes en el juego
* Entradas: 
    Arreglo de instancias de objeto Projectile (proyectiles)
    Número entero que indica la cantidad de proyectiles activos en el juego
* Salidas: 
    Ninguna
*************************************************************************/
void projectiles_clear(Projectile projectiles[], int count)
{
    //Mientras i sea menor a la cantidad de maxima de proyectiles admitida en el juego
    for (int i = 0; i < count; ++i) {
        //Para el proyectil guardado en el índice i actual:
        projectiles[i].active = 0;  //Lo marca como inactivo
        projectiles[i].position.x = 0;  //Reestablece su coordenada x
        projectiles[i].position.y = 0;  //Reestablece su coordenada y
        projectiles[i].velocity.x = 0;  //Reestablece su velocidad horizontal 
        projectiles[i].velocity.y = 0;  //Reestablece su velocidad vertical
    }
}

/************************************************************************
* Función: 
    proyectiles_spawn
* Descripción: 
    Genera la cantidad de proyectiles que se requiera
* Entradas: 
    Arreglo de instancias de objeto Projectile (proyectiles)
    Número entero que indica la cantidad de proyectiles activos en el juego
    Vector con coordenadas (x,y) desde las cuales se desea se genere el 
        proyectil
    Vector con velocidades (x,y) que desea tenga el proyectil
* Salidas: 
    Ninguna
*************************************************************************/
void projectiles_spawn(Projectile projectiles[], int count, Vec2i position, Vec2i velocity)
{
    //Mientras i sea menor a la cantidad de maxima de power-ups admitida en el juego
    for (int i = 0; i < count; ++i) {
        //Si el proyectil en el índice actual se encuentra inactivo
        if (!projectiles[i].active) {
            projectiles[i].active = 1;  //Lo marca como activo
            projectiles[i].position = position; //Establece su coordenada de aparición de acuerdo a la ingresada
            projectiles[i].velocity = velocity; //Establece sus velocidades de acuerdo al vector de velocidades ingresado
            return;
        }
    }
}

/************************************************************************
* Función: 
    projectiles_update
* Descripción: 
    Actualiza el estado de los proyectiles activos en el juego
* Entradas: 
    Arreglo de instancias de objeto Projectile (proyectiles)
    Número entero que indica la cantidad de proyectiles activos en el juego
    Número entero que indica el número de frame actual del juego
    Número entero que indica el intervalo (en frames) entre cada 
        movimiento
* Salidas: 
    Ninguna
*************************************************************************/
void projectiles_update(Projectile projectiles[], int count, int frame, int move_interval)
{
    //Si el intervalo de movimiento es igual o menor a cero, o si el residuo de dividir
    //  el frame actual entre el intervalo de movimiento no es iguaol a cero
    if (move_interval <= 0 || frame % move_interval != 0) {
        return; //Sale de la función
    }

    //Mientras i sea menor a la cantidad de maxima de power-ups admitida en el juego
    for (int i = 0; i < count; ++i) {
        //Si el proyectil en el índice actual se encuentra inactivo
        if (!projectiles[i].active) {
            //Pasa a la siguiente iteración, es decir, pasa procesar el siguiente proyectil guardado en el arreglo
            continue;
        }

        //Si este se encuentra activo, incrementa una vez sus coordenadas de acuerdo a las velocidades con las que cuenta este
        projectiles[i].position.x += projectiles[i].velocity.x;
        projectiles[i].position.y += projectiles[i].velocity.y;

        //Si el proyectil actual ya no se encuentra dentro del área de juego
        if (!inside_board(projectiles[i].position)) {
            projectiles[i].active = 0;  //Procede a marcarlo como inactivo
        }
    }
}
