#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include "config.h"
#include "game.h"
#include "input.h"
#include "render.h"
#include "sound.h"

/************************************************************************
* Función: 
    sleep_ms
* Descripción: 
    Tiempo de espera entre cada refresco de pantalla
* Entradas: 
    Tiempo en milisegundos que se desea "congelar" el proceso/programa
* Salidas: 
    Ninguna
*************************************************************************/
static void sleep_ms(int milliseconds)
{
//Si el SO  es windows
#ifdef _WIN32
    Sleep((DWORD)milliseconds);

//De lo contrario
#else
    struct timespec duration;
    duration.tv_sec = milliseconds / 1000;
    duration.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
    nanosleep(&duration, 0);
#endif
}

/************************************************************************
* Función: 
    main
* Descripción: 
    Encargada de la ejecución del programa
* Entradas: 
    Ninguna
* Salidas: 
    0 -> El programa ha sido ejecutado con éxito
*************************************************************************/
int main(void)
{
    GameState game; //Declara variable tipo GameState
    game_init(&game);   //Inicializa el juego
    render_init();  //Incializa el motor de renderizado
    sound_init(); //Inicializa el motor de sonido
    sound_update_music(game.screen); //Actualiza la música de fondo según la pantalla actual del juego

    //Mientras el juego esté corriendo
    while (game.running) {
        render_draw(&game); //Renderiza el estado actual del juego
        int input = input_poll();   //Revisa cual ha sido la última entrada por parte del usuario
        game_update(&game, input);  //Actuliza el estadod el juego
        sound_update_music(game.screen); //Actualiza la música de fondo según la pantalla actual del juego
        sleep_ms(FRAME_DELAY_MS);   //Congela el juego por algunos milisegundos
    }

    render_draw(&game); //Renderiza el último frame del juego
    sound_shutdown();  //"Apaga" el motor de sonido del juego
    render_shutdown();  //"Apaga" el motor de renderizado del juego
    puts("Gracias por jugar."); //Despliega mensaje en consola
    return 0;   //Mensaje de que el programa ha sido ejecutado correctamente
}
