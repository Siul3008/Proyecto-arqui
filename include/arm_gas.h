#ifndef RECCA_ARM_GAS_H
#define RECCA_ARM_GAS_H

#include "types.h"

#if defined(RECCA_USE_ARM_GAS)
void recca_fill_board_gas(char *board, int total_cells, int value);
int recca_color_for_char_gas(int value);
void recca_put_projectiles_on_board_gas(char *board, const Projectile *projectiles, int count, int value);
void recca_put_effects_on_board_gas(char *board, const Effect *effects, int count, int value);
int recca_positions_overlap_gas(int ax, int ay, int bx, int by);
int recca_ranges_overlap_gas(int left_a, int right_a, int left_b, int right_b);
int recca_projectile_hits_player_gas(int shot_x, int shot_y, int player_x, int player_y);
int recca_projectile_hits_enemy_gas(int shot_x, int shot_y, int enemy_x, int enemy_y, int enemy_type);
int recca_player_hits_enemy_gas(int player_x, int player_y, int enemy_x, int enemy_y, int enemy_type);
int recca_capped_timer_add_gas(int current_timer, int added_timer, int max_timer);
#endif

#endif
