#ifndef RECCA_SOUND_H
#define RECCA_SOUND_H

#include "types.h"

void sound_init(void);
void sound_shutdown(void);
void sound_update_music(GameScreen screen);
void sound_shot(void);
void sound_enemy_destroyed(void);
void sound_powerup(void);
void sound_damage(void);
void sound_charge_bomb(void);
void sound_boss_defeated(void);
void sound_extra_life(void);
void sound_konami(void);
void sound_game_over(void);

#endif
