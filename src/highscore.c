#include "highscore.h"

#include "config.h"

#include <stdio.h>
#include <string.h>

static void copy_name(char destination[], const char *source)
{
    strncpy(destination, source, PLAYER_NAME_MAX_LENGTH);
    destination[PLAYER_NAME_MAX_LENGTH] = '\0';
}

void highscores_clear(HighScoreEntry scores[], int count)
{
    for (int i = 0; i < count; ++i) {
        copy_name(scores[i].name, "---");
        scores[i].score = 0;
        scores[i].rank = 0;
        scores[i].bosses = 0;
    }
}

void highscores_load(HighScoreEntry scores[], int count, const char *path)
{
    FILE *file = fopen(path, "r");

    highscores_clear(scores, count);

    if (file == 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        char name[PLAYER_NAME_MAX_LENGTH + 1];
        int score = 0;
        int rank = 0;
        int bosses = 0;

        if (fscanf(file, "%10s %d %d %d", name, &score, &rank, &bosses) != 4) {
            break;
        }

        copy_name(scores[i].name, name);
        scores[i].score = score;
        scores[i].rank = rank;
        scores[i].bosses = bosses;
    }

    fclose(file);
}

void highscores_save(const HighScoreEntry scores[], int count, const char *path)
{
    FILE *file = fopen(path, "w");

    if (file == 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        if (strcmp(scores[i].name, "---") == 0) {
            continue;
        }

        fprintf(file,
                "%s %d %d %d\n",
                scores[i].name,
                scores[i].score,
                scores[i].rank,
                scores[i].bosses);
    }

    fclose(file);
}

void highscores_add(HighScoreEntry scores[], int count, HighScoreEntry entry)
{
    for (int i = 0; i < count; ++i) {
        if (entry.score >= scores[i].score) {
            for (int j = count - 1; j > i; --j) {
                scores[j] = scores[j - 1];
            }
            scores[i] = entry;
            return;
        }
    }
}
