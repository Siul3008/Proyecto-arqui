#include "highscore.h"

#include "config.h"

#include <stdio.h>
#include <string.h>

/************************************************************************
* Función: 
    copy_name
* Descripción: 
    Copia una cadena de caracteres a un buffer de destino
* Entradas: 
    Buffer de caracteres (Destino)
    Puntero a cadena de caracteres
* Salidas: 
    Ninguna
*************************************************************************/
static void copy_name(char destination[], const char *source)
{
    strncpy(destination, source, PLAYER_NAME_MAX_LENGTH);   //Copia al destino la cadena de caracteres de origen
    destination[PLAYER_NAME_MAX_LENGTH] = '\0'; //Agrega un caracter nulo al final del buffer de destino
}

/************************************************************************
* Función: 
    highscores_clear
* Descripción: 
    Limpia la tabla de puntuaciones guardadas
* Entradas: 
    Arreglo con objetos tipo HighScoreEntry
    Cantidad de puntajes guardados
* Salidas: 
    Ninguna
*************************************************************************/
void highscores_clear(HighScoreEntry scores[], int count)
{   
    //Mientras i sea menor a la cantidad total de puntajes guardados
    for (int i = 0; i < count; ++i) {
        copy_name(scores[i].name, "---");   //Copia "---" al campo de nombre del jugador en el objeto guardado en el índice "i" del arreglo proporcionado
        scores[i].score = 0;    //Establece en cero el campo de puntaje en el objeto guardado en el índice "i" del arreglo proporcionado
        scores[i].rank = 0;     //Establece en cero el campo de rango o nivel alcanzado por el jugador en el objeto guardado en el índice "i" del arreglo proporcionado
        scores[i].bosses = 0;   //Establece en cero el campo de número de jefes derrotados en el objeto guardado en el índice "i" del arreglo proporcionado
    }
}

/************************************************************************
* Función: 
    highscores_load
* Descripción: 
    Carga la tabla de puntuaciones guardadas
* Entradas: 
    Arreglo con objetos tipo HighScoreEntry
    Número entero que incia la cantidad de puntajes guardados
    Cadena de caracteres que indica la dirección donde se encuentra el 
        archivo que contiene los datos de tabla de puntuación 
* Salidas: 
    Ninguna
*************************************************************************/
void highscores_load(HighScoreEntry scores[], int count, const char *path)
{
    //Define el puntero tipo FILE y "abre" el archivo que se encuentra en la dirección especificada, esto lo hace en modo lectura
    FILE *file = fopen(path, "r");  

    highscores_clear(scores, count);    //Limpia la tabla de puntuaciones cargada en memoria

    //Si ha habido un error al cargar el archivo
    if (file == 0) {
        return; //Sale de la función
    }

    //Mientras i sea menor a la cantidad de puntajes guardados
    for (int i = 0; i < count; ++i) {
        char name[PLAYER_NAME_MAX_LENGTH + 1];  //Inicializa una variable de caracteres para que esta tenga una longitud igual a la del nombre ingresado + 1 caracter (el nulo)
        int score = 0;  //Inicializa una variable de puntaje y la establece en 0 
        int rank = 0;   //Inicializa una variable de rango o nivel y la establece en 0
        int bosses = 0; //Inicializa una variable de cantidad de jefes derrotados y la establece en 0

        /*Si al revisar la línea actual del archivo, este no ha leído correctamente los siguientes datos:
            10 caracteres del primer dato   -> Copia a variable name
            Número entero como segundo dato -> Copia a variable score
            Número entero como tercer dato  -> Copia a variable rank
            Número entero como cuarto dato  -> Copia a variable bosses
        Sale del ciclo for*/
        if (fscanf(file, "%10s %d %d %d", name, &score, &rank, &bosses) != 4) {
            break;
        }

        //Si todo se ha leído correctamente, copia datos a a objeto tipo HighScoreEntry en el indice del arreglo proporcionado:
        copy_name(scores[i].name, name);    //Copia el nombre
        scores[i].score = score;    //Copia el puntaje 
        scores[i].rank = rank;      //Copia el rango o nivel
        scores[i].bosses = bosses;  //Copia el número de jefes derrotados
    }

    fclose(file); //"Cierra" el archivo
}

/************************************************************************
* Función: 
    highscores_save
* Descripción: 
    Guarda la tabla de puntuaciones cargada en memoria
* Entradas: 
    Arreglo con objetos tipo HighScoreEntry
    Número entero que incia la cantidad de puntajes guardados
    Cadena de caracteres que indica la dirección donde se encuentra el 
        archivo que contiene los datos de tabla de puntuación 
* Salidas: 
    Ninguna
*************************************************************************/
void highscores_save(const HighScoreEntry scores[], int count, const char *path)
{
    //Define el puntero tipo FILE y "abre" el archivo que se encuentra en la dirección especificada, esto lo hace en modo escritura
    FILE *file = fopen(path, "w");

    //Si ha habido un error al cargar el archivo
    if (file == 0) {
        return; //Sale de la función
    }

    //Mientras i sea menor a la cantidad de puntajes guardados
    for (int i = 0; i < count; ++i) {
        //Si el campo de nombre es igual a "---"
        if (strcmp(scores[i].name, "---") == 0) {
            continue;   //Pasa a la siguiente iteración, es decir, al siguiente nombre y puntaje
        }

        //Escribe en la línea actual con el siguiente formato:
        //  Nombre_jugador Puntaje Nivel/Rango #_jefes_derrotados
        fprintf(file,
                "%s %d %d %d\n",
                scores[i].name,
                scores[i].score,
                scores[i].rank,
                scores[i].bosses);
    }

    fclose(file);   //"Cierra" el archivo
}

/************************************************************************
* Función: 
    highscores_add
* Descripción: 
    Añade una puntuación adicional a la tabla
* Entradas: 
    Arreglo con objetos tipo HighScoreEntry
    Número entero que incia la cantidad de puntajes guardados
    Objeto tipo HighScoreEntry
* Salidas: 
    Ninguna
*************************************************************************/
void highscores_add(HighScoreEntry scores[], int count, HighScoreEntry entry)
{
    //Mientras i sea menor a la cantidad de puntajes guardados
    for (int i = 0; i < count; ++i) {
        //Si el puntaje de la nueva entrada es mayor al de la entrada en el índice actual del arreglo
        if (entry.score >= scores[i].score) {
            //Mientras j sea mayor a i, donde j será igual a la cantidad de puntuaciones guardadas -1
            for (int j = count - 1; j > i; --j) {
                //La entrada en el índice j actual será igual a la ubicada en el índice anterior
                scores[j] = scores[j - 1];
            }
            //Además, la entrada en el índice i actual será igual a la nueva entrada
            scores[i] = entry;
            return;
        }
    }
}
