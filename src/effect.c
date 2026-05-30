#include "effect.h"

void effect_clear(Effect effects[], int count)
{
    for (int i = 0; i < count; ++i) {
        effects[i].active = 0;
        effects[i].position.x = 0;
        effects[i].position.y = 0;
        effects[i].timer = 0;
    }
}

void effect_spawn(Effect effects[], int count, Vec2i position, int duration)
{
    for (int i = 0; i < count; ++i) {
        if (!effects[i].active) {
            effects[i].active = 1;
            effects[i].position = position;
            effects[i].timer = duration;
            return;
        }
    }
}

void effect_update(Effect effects[], int count)
{
    for (int i = 0; i < count; ++i) {
        if (!effects[i].active) {
            continue;
        }

        effects[i].timer -= 1;

        if (effects[i].timer <= 0) {
            effects[i].active = 0;
        }
        
    }
}
