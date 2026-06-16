#include "sound.h"

#include "config.h"

#if USE_SDL_AUDIO
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL_mixer.h>

static int audio_ready = 0;
static Mix_Music *music_menu = 0;
static Mix_Chunk *sfx_powerup = 0;
static Mix_Chunk *sfx_shot = 0;
static Mix_Chunk *sfx_game_over_voice = 0;
static Mix_Chunk *sfx_damage = 0;
static Mix_Chunk *sfx_explosion = 0;
static Mix_Chunk *sfx_bonus = 0;
static Uint32 last_shot_tick = 0;
static GameScreen music_screen = GAME_SCREEN_GAME_OVER;

/************************************************************************
* Función: 
    load_chunk
* Descripción: 
    Carga un archivo de sonido y lo convierte en un Mix_Chunk para su reproducción
* Entradas: 
    Cadena de caracteres que indica la ruta del archivo de sonido a cargar
* Salidas: 
    Puntero a Mix_Chunk que representa el sonido cargado, o 0 si hubo un error
*************************************************************************/
static Mix_Chunk *load_chunk(const char *path)
{
    return Mix_LoadWAV(path);
}

/************************************************************************
* Función: 
    play_chunk
* Descripción: 
    Reproduce un sonido representado por un Mix_Chunk
* Entradas: 
    Puntero a Mix_Chunk que representa el sonido a reproducir
* Salidas: 
    Ninguna
*************************************************************************/
static void play_chunk(Mix_Chunk *chunk)
{
    if (audio_ready && chunk != 0) {
        Mix_PlayChannel(-1, chunk, 0);
    }
}

/************************************************************************
* Función: 
    sound_init
* Descripción: 
    Inicializa el sistema de sonido del juego, cargando los archivos de música y efectos de sonido necesarios
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_init(void)
{
#if SOUND_ENABLED
    SDL_SetMainReady();
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }

    Mix_AllocateChannels(16);
    Mix_Volume(-1, MIX_MAX_VOLUME / 2);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

    music_menu = Mix_LoadMUS("assets/music/sonido menu.wav");
    sfx_powerup = load_chunk("assets/sfx/8-bit-powerup.wav");
    sfx_shot = load_chunk("assets/sfx/shot.wav");
    sfx_game_over_voice = load_chunk("assets/sfx/game-over.wav");
    sfx_damage = load_chunk("assets/sfx/damage.wav");
    sfx_explosion = load_chunk("assets/sfx/explosion.wav");
    sfx_bonus = load_chunk("assets/sfx/game-bonus.wav");

    audio_ready = 1;
    music_screen = GAME_SCREEN_GAME_OVER;
#endif
}

/************************************************************************
* Función: 
    sound_update_music
* Descripción: 
    Actualiza la música de fondo del juego según la pantalla actual del juego
* Entradas: 
    Enumerado GameScreen que indica la pantalla actual del juego
* Salidas: 
    Ninguna
*************************************************************************/
void sound_update_music(GameScreen screen)
{
#if SOUND_ENABLED
    if (!audio_ready || screen == music_screen) {
        return;
    }

    music_screen = screen;
    if (screen == GAME_SCREEN_MENU && music_menu != 0) {
        Mix_PlayMusic(music_menu, -1);
    } else {
        Mix_HaltMusic();
    }
#endif
}

/************************************************************************
* Función: 
    sound_shutdown
* Descripción: 
    "Apaga" el sistema de sonido del juego, liberando los recursos utilizados por la música y los efectos de sonido
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_shutdown(void)
{
#if SOUND_ENABLED
    if (!audio_ready) {
        return;
    }

    Mix_HaltMusic();
    Mix_FreeMusic(music_menu);
    Mix_FreeChunk(sfx_powerup);
    Mix_FreeChunk(sfx_shot);
    Mix_FreeChunk(sfx_game_over_voice);
    Mix_FreeChunk(sfx_damage);
    Mix_FreeChunk(sfx_explosion);
    Mix_FreeChunk(sfx_bonus);
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    audio_ready = 0;
#endif
}

/************************************************************************
* Función: 
    sound_shot
* Descripción: 
    Reproduce el sonido de disparo del jugador
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_shot(void)
{
    Uint32 now = SDL_GetTicks();
    if (now - last_shot_tick < 80) {
        return;
    }

    last_shot_tick = now;
    play_chunk(sfx_shot);
}

/************************************************************************
* Función: 
    sound_enemy_destroyed
* Descripción: 
    Reproduce el sonido de destrucción de un enemigo
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_enemy_destroyed(void)
{
    play_chunk(sfx_explosion);
}

/************************************************************************
* Función: 
    sound_powerup
* Descripción: 
    Reproduce el sonido de obtención de un power-up
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_powerup(void)
{
    play_chunk(sfx_powerup);
}

/************************************************************************
* Función: 
    sound_damage
* Descripción: 
    Reproduce el sonido de daño recibido por el jugador
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_damage(void)
{
    play_chunk(sfx_damage);
}

/************************************************************************
* Función: 
    sound_charge_bomb
* Descripción: 
    Reproduce el sonido de la bomba cargada del jugador
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_charge_bomb(void)
{
    play_chunk(sfx_explosion);
}

/************************************************************************
* Función: 
    sound_boss_defeated
* Descripción: 
    Reproduce el sonido de derrota de un jefe
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_boss_defeated(void)
{
    play_chunk(sfx_explosion);
}

/************************************************************************
* Función: 
    sound_extra_life
* Descripción: 
    Reproduce el sonido de obtención de una vida extra
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_extra_life(void)
{
    play_chunk(sfx_bonus);
}

/************************************************************************
* Función:  
    sound_konami
* Descripción: 
    Reproduce el sonido de activación del código Konami
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_konami(void)
{
    play_chunk(sfx_bonus);
}

/************************************************************************
* Función: 
    sound_game_over
* Descripción: 
    Reproduce el sonido de la voz de "Game Over"
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void sound_game_over(void)
{
    play_chunk(sfx_game_over_voice);
}


#else
#include <curses.h>

static void play_beep(void)
{
#if SOUND_ENABLED
    beep();
#endif
}

void sound_init(void) {}
void sound_shutdown(void) {}
void sound_update_music(GameScreen screen) { (void)screen; }
void sound_shot(void) { play_beep(); }
void sound_enemy_destroyed(void) { play_beep(); }
void sound_powerup(void) { play_beep(); }
void sound_damage(void) { play_beep(); }
void sound_charge_bomb(void) { play_beep(); }
void sound_boss_defeated(void) { play_beep(); }
void sound_extra_life(void) { play_beep(); }
void sound_konami(void) { play_beep(); }
void sound_game_over(void) { play_beep(); }
#endif
