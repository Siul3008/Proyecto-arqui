#ifndef RECCA_ARM_GAS_H
#define RECCA_ARM_GAS_H

#include "types.h"

#if defined(RECCA_USE_ARM_GAS)
//Definiciones de funciones de renderizado optimizadas para ARM
void recca_fill_board_gas(char *board, int total_cells, int value);
int recca_color_for_char_gas(int value);
void recca_put_projectiles_on_board_gas(char *board, const Projectile *projectiles, int count, int value);
void recca_put_effects_on_board_gas(char *board, const Effect *effects, int count, int value);
#endif

#endif
