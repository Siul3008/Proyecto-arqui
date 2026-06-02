#ifndef RECCA_EFFECT_H
#define RECCA_EFFECT_H

#include "types.h"

void effect_clear(Effect effects[], int count);
void effect_spawn(Effect effects[], int count, Vec2i position, int duration);
void effect_update(Effect effects[], int count);

#endif