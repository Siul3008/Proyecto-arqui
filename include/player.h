#ifndef RECCA_PLAYER_H
#define RECCA_PLAYER_H

#include "types.h"

void player_init(Player *player);
void player_update(Player *player, int input_mask, Projectile player_shots[], int shot_count);

#endif
