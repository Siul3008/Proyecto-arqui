#include "timer.h"
#include "types.h"
#include "time.h"
#include "config.h"
#include "game.h"

#include <time.h>
#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

void timer_init(Timer *t, int module_index) {
    t->timed_module = module_index;
    t->last_time = 0.0;
    t->total_time = 0.0;
    t->count = 0;
}

void timer_reset(Timer *t) {
    t->last_time = 0.0;
    t->total_time = 0.0;
    t->count = 0;    
}

void timer_start(const GameState *game, Timer *t) {
    if (game->screen == GAME_SCREEN_PLAYING) {
        t->last_time = 0.0;
        clock_gettime(CLOCK_MONOTONIC, &(t->start));    //Obtiene el timestamp correspondiente al inicio de la medición
    }
}

void timer_end(const GameState *game, Timer *t) {
    //Para evitar que se llene demasiado la memoria, cada cierto tiempo reestablece el contador y el tiempo total acumulado
    if (t->count >= TIMER_UPDATE_INTERVAL) {
        t->total_time = 0.0;
        t->count = 0;
    }

    //Procede al calculo del tiempo SOLO si hay una partida en curso
    if (game->screen == GAME_SCREEN_PLAYING) {
        struct timespec current_end; 
        clock_gettime(CLOCK_MONOTONIC, &current_end);   //Obtiene el timestamp correspondiente al final de la medición

        //Calcula, en segundos la diferencia con de la muestra final con respecto a la inicial
        double diff_sec = (double)(current_end.tv_sec - t->start.tv_sec);
        double diff_nsec = (double)(current_end.tv_nsec - t->start.tv_nsec);

        //El resultado en segundos se multiplica por 1000.0, para así pasarlo a milisegundos
        t->last_time = (diff_sec + (diff_nsec / 1000000000.0)) * 1000.0;

        t->total_time += t->last_time;  //Le suma el último tiempo obtenido al aculumado de tiempo total que se lleva guardado
        t->count++; //Incrementa una vez el contador de ciclos que se han medido

    }
}

void timer_report(Timer *t) {
    //Actuliza la informacion en pantalla SOLO si ha pasado la cantidad de ciclos indicada en config.h, y si el numero de módulo se encuentra dentro del rango establecido (1 o 0)
    if (t->timed_module < 0 || t->timed_module > 1) {
        return;
    }

    //Obtiene el numero de fila y columna desde los cuales se desea empezar a imprimir la información
    int row = GAME_HEIGHT/3 + 10 + (t->timed_module * 4);
    int col = GAME_WIDTH + 10;

    //Obtiene el nombre el módulo que ha sido monitoreado
    const char *module_name = (t->timed_module == 0) ? "render.c" : "collision.c";

    //Imprime el nombre del módulo que acaba de ser medido
    mvprintw(row++, col, "MODULO: %s", module_name);
    mvprintw(row++, col, "  Ultima ejecucion : %.5f milisegundos", t->last_time);   //Imprime el último tiempo de ejecución
    mvprintw(row++, col, "  Tiempo Promedio  : %.5f milisegundos", t->total_time / t->count);   //Imprime el tiempo de ejecución promedio  
}

void timers_render(Timer *t1, Timer *t2) {
    timer_report(t1);
    timer_report(t2);
    refresh();
}
