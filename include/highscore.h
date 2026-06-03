#ifndef RECCA_HIGHSCORE_H
#define RECCA_HIGHSCORE_H

#include "types.h"

void highscores_clear(HighScoreEntry scores[], int count);
void highscores_load(HighScoreEntry scores[], int count, const char *path);
void highscores_save(const HighScoreEntry scores[], int count, const char *path);
void highscores_add(HighScoreEntry scores[], int count, HighScoreEntry entry);

#endif
