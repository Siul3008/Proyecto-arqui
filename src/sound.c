#include "sound.h"

#include "config.h"

#if USE_SDL_AUDIO
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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

static Mix_Chunk *load_chunk(const char *path)
{
    return Mix_LoadWAV(path);
}

static void play_chunk(Mix_Chunk *chunk)
{
    if (audio_ready && chunk != 0) {
        Mix_PlayChannel(-1, chunk, 0);
    }
}

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

void sound_shot(void)
{
    Uint32 now = SDL_GetTicks();
    if (now - last_shot_tick < 80) {
        return;
    }

    last_shot_tick = now;
    play_chunk(sfx_shot);
}

void sound_enemy_destroyed(void)
{
    play_chunk(sfx_explosion);
}

void sound_powerup(void)
{
    play_chunk(sfx_powerup);
}

void sound_damage(void)
{
    play_chunk(sfx_damage);
}

void sound_charge_bomb(void)
{
    play_chunk(sfx_explosion);
}

void sound_boss_defeated(void)
{
    play_chunk(sfx_explosion);
}

void sound_extra_life(void)
{
    play_chunk(sfx_bonus);
}

void sound_konami(void)
{
    play_chunk(sfx_bonus);
}

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
