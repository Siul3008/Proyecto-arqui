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

static void sleep_ms(int milliseconds)
{
#ifdef _WIN32
    Sleep((DWORD)milliseconds);
#else
    struct timespec duration;
    duration.tv_sec = milliseconds / 1000;
    duration.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
    nanosleep(&duration, 0);
#endif
}

int main(void)
{
    GameState game;
    game_init(&game);
    render_init();

    while (game.running) {
        render_draw(&game);
        int input = input_poll();
        game_update(&game, input);
        sleep_ms(FRAME_DELAY_MS);
    }

    render_draw(&game);
    render_shutdown();
    puts("Gracias por jugar.");
    return 0;
}
