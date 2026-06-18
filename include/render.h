#ifndef RECCA_RENDER_H
#define RECCA_RENDER_H

#include "game.h"

void render_init(void);
void render_shutdown(void);
void render_clear_screen(void);
void render_draw(const GameState *game);

#endif
