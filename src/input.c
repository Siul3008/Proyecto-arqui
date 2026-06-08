#include "input.h"

#include "types.h"

#include <ncursesw/curses.h>

//Crea una variable global que indica cual ha sido el último caracter ingresado
static int last_character = ERR;


/************************************************************************
* Función: 
    input_poll
* Descripción: 
    Lee e interpreta cual ha sido la última tecla presionada por el 
        jugador
* Entradas: 
    Ninguna
* Salidas: 
    Número entero que indica cual ha sido el índice del caracter ingresado
*************************************************************************/
int input_poll(void)
{
    //Inicializa variable input en INPUT_NONE 
    //  Ya si la tecla presionada corresponde a una de las que necesita el programa, este valor cambiará correspondientemente
    int input = INPUT_NONE;
    int character = getch();    //Obtiene el caracter correspondiente a la tecla presionada
    last_character = character; //Guarda en la variable global, el caracter que acaba de ser ingresado

    switch (character) {
    case '\n':
    case '\r':
    case KEY_ENTER:
        input |= INPUT_START;
        break;
    case KEY_LEFT:
    case 'a':
    case 'A':
        input |= INPUT_LEFT;
        break;
    case KEY_RIGHT:
    case 'd':
    case 'D':
        input |= INPUT_RIGHT;
        break;
    case KEY_UP:
    case 'w':
    case 'W':
        input |= INPUT_UP;
        break;
    case KEY_DOWN:
    case 's':
    case 'S':
        input |= INPUT_DOWN;
        break;
    case KEY_BACKSPACE:
    case 127:
    case '\b':
        input |= INPUT_BACKSPACE;
        break;
    case ' ':
        input |= INPUT_FIRE;
        break;
    case 'r':
    case 'R':
        input |= INPUT_RESTART;
        break;
    case 'p':
    case 'P':
        input |= INPUT_PAUSE;
        break;
    case 'h':
    case 'H':
        input |= INPUT_HELP;
        break;
    case 'q':
    case 'Q':
        input |= INPUT_QUIT;
        break;
    default:
        break;
    }

    return input;   //Retorna el caracter ingresado
}

/************************************************************************
* Función: 
    input_last_character
* Descripción: 
    Retorna el índice del último caracter ingresado
* Entradas: 
    Ninguna
* Salidas: 
    Número entero que indica cual ha sido el índice del último 
        caracter ingresado
*************************************************************************/
int input_last_character(void)
{
    return last_character;
}
