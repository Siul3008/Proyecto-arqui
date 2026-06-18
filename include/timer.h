#ifndef RECCA_TIMER_H
#define RECCA_TIMER_H

#include "types.h"
#include "game.h"
#include <time.h>
#include "config.h"

void timer_init(Timer *t, int module_index);
void timer_start(const GameState *game, Timer *t);
void timer_reset(Timer *t);
void timer_end(const GameState *game, Timer *t);
void timer_report(Timer *t);
void timers_render(Timer *t1, Timer *t2);

#endif