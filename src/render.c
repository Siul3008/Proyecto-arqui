#include "render.h"

#include <curses.h>
#include <locale.h>

#include "config.h"

enum {
    COLOR_PAIR_PLAYER = 1,
    COLOR_PAIR_PLAYER_SHOT,
    COLOR_PAIR_ENEMY,
    COLOR_PAIR_ENEMY_SHOT,
    COLOR_PAIR_BORDER,
    COLOR_PAIR_TEXT
};

static void fill_board(char board[GAME_HEIGHT][GAME_WIDTH])
{
    for (int y = 0; y < GAME_HEIGHT; ++y) {
        for (int x = 0; x < GAME_WIDTH; ++x) {
            board[y][x] = ' ';
        }
    }
}

static void put_char(char board[GAME_HEIGHT][GAME_WIDTH], int x, int y, char value)
{
    if (x >= 0 && x < GAME_WIDTH && y >= 0 && y < GAME_HEIGHT) {
        board[y][x] = value;
    }
}

static int color_for_char(char value)
{
    switch (value) {
    case '|':
        return COLOR_PAIR_PLAYER_SHOT;
    case 'v':
        return COLOR_PAIR_ENEMY;
    case 'o':
        return COLOR_PAIR_ENEMY_SHOT;
    default:
        return COLOR_PAIR_TEXT;
    }
}

void render_init(void)
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        init_pair(COLOR_PAIR_PLAYER, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_PAIR_PLAYER_SHOT, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_PAIR_ENEMY, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_PAIR_ENEMY_SHOT, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_PAIR_BORDER, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_PAIR_TEXT, COLOR_WHITE, COLOR_BLACK);
    }
}

void render_shutdown(void)
{
    curs_set(1);
    nodelay(stdscr, FALSE);
    endwin();
}

void render_clear_screen(void)
{
    erase();
}

void render_draw(const GameState *game)
{
    char board[GAME_HEIGHT][GAME_WIDTH];
    fill_board(board);

    for (int i = 0; i < MAX_PLAYER_SHOTS; ++i) {
        if (game->player_shots[i].active) {
            put_char(board, game->player_shots[i].position.x, game->player_shots[i].position.y, '|');
        }
    }

    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        if (game->enemy_shots[i].active) {
            put_char(board, game->enemy_shots[i].position.x, game->enemy_shots[i].position.y, 'o');
        }
    }

    for (int i = 0; i < MAX_ENEMIES; ++i) {
        if (game->enemies[i].active) {
            put_char(board, game->enemies[i].position.x, game->enemies[i].position.y, 'v');
        }
    }

    render_clear_screen();

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    mvprintw(0, 0, "Summer Carnival '92: Recca - texto");
    mvprintw(1, 0, "Score: %06d  Lives: %d  Controls: W/A/S/D, arrows, Space, Q",
             game->player.score, game->player.lives);
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));
    mvaddch(2, 0, '+');
    for (int x = 0; x < GAME_WIDTH; ++x) {
        mvaddch(2, x + 1, '-');
    }
    mvaddch(2, GAME_WIDTH + 1, '+');
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

    for (int y = 0; y < GAME_HEIGHT; ++y) {
        attron(COLOR_PAIR(COLOR_PAIR_BORDER));
        mvaddch(y + 3, 0, '|');
        mvaddch(y + 3, GAME_WIDTH + 1, '|');
        attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

        for (int x = 0; x < GAME_WIDTH; ++x) {
            char value = board[y][x];
            int color = color_for_char(value);
            attron(COLOR_PAIR(color));
            mvaddch(y + 3, x + 1, value);
            attroff(COLOR_PAIR(color));
        }
    }

    attron(COLOR_PAIR(COLOR_PAIR_PLAYER));
    mvprintw(game->player.position.y + 3, game->player.position.x, "/⮝\\");
    attroff(COLOR_PAIR(COLOR_PAIR_PLAYER));

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));
    mvaddch(GAME_HEIGHT + 3, 0, '+');
    for (int x = 0; x < GAME_WIDTH; ++x) {
        mvaddch(GAME_HEIGHT + 3, x + 1, '-');
    }
    mvaddch(GAME_HEIGHT + 3, GAME_WIDTH + 1, '+');
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

    if (game->game_over) {
        attron(COLOR_PAIR(COLOR_PAIR_ENEMY));
        mvprintw(GAME_HEIGHT + 5, 0, "GAME OVER");
        attroff(COLOR_PAIR(COLOR_PAIR_ENEMY));
    }

    refresh();
}
