#include "effect.h"

/************************************************************************
* Función: 
    effect_clear
* Descripción: 
    Limpia los efectos que hay en pantalla
* Entradas: 
    Arreglo de efectos en pantalla
    Cantidad de efectos en pantalla
* Salidas: 
    Ninguna
*************************************************************************/
void effect_clear(Effect effects[], int count)
{
    //Recorre la lista de efectos en pantalla
    for (int i = 0; i < count; ++i) {
        effects[i].active = 0;      //Desactiva el efecto
        effects[i].position.x = 0;  //Reinicia en 0 sus coordenadas "x" y "y"
        effects[i].position.y = 0;
        effects[i].timer = 0;       //Agota el temporizador de tiempo que debe aparecer en pantalla
    }
}

/************************************************************************
* Función: 
    effect_spawn
* Descripción: 
    Genera nuevos efectos en pantalla
* Entradas: 
    Arreglo de efectos en pantalla
    Cantidad de efectos en pantalla
    Vector (x,y) con las coordenadas que debe tener el efecto
    Duración (en frames) que debe tener el efecto en pantalla
* Salidas: 
    Ninguna
*************************************************************************/
void effect_spawn(Effect effects[], int count, Vec2i position, int duration)
{
    //Recorre la lista de efectos en pantalla
    for (int i = 0; i < count; ++i) {
        //Si el efecto se encuentra inactivo
        if (!effects[i].active) {
            effects[i].active = 1;          //Activa el efecto en pantalla
            effects[i].position = position; //Le asigna la posición indicada al nuevo efecto
            effects[i].timer = duration;    //Le asigna la duración indicada al nuevo efecto
            return;
        }
    }
}

/************************************************************************
* Función: 
    effect_update
* Descripción: 
    Actualiza el estado de los efectos que hay en pantalla
* Entradas: 
    Arreglo de efectos en pantalla
    Cantidad de efectos en pantalla
* Salidas: 
    Ninguna
*************************************************************************/
void effect_update(Effect effects[], int count)
{   
    //Recorre la lista de efectos en pantalla
    for (int i = 0; i < count; ++i) {
        //Si el efecto se encuentra inactivo, pasa al siguiente efecto de la lista
        if (!effects[i].active) {
            continue;
        }

        //De lo contrario, decrementa el temporizador de tiempo que debe estar en pantalla
        effects[i].timer -= 1;

        //Si el temporizador de tiempo que debe estar el efecto en pantalla es menor o igual a cero
        if (effects[i].timer <= 0) {
            //Marca el efecto como inactivo
            effects[i].active = 0;
        }
        
    }
}
