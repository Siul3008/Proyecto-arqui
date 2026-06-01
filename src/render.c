#include "render.h"

#include <curses.h>
#include <locale.h>
#include <string.h>

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
    case '*':
        return COLOR_PAIR_PLAYER_SHOT;
    case 'v':
    case 'd':
    case 'z':
    case 'f':
        return COLOR_PAIR_ENEMY;
    case 'o':
        return COLOR_PAIR_ENEMY_SHOT;
    default:
        return COLOR_PAIR_TEXT;
    }
}

static char enemy_char_for_type(EnemyType type)
{
    switch (type) {
    case ENEMY_STRAIGHT:
        return 'v';
    case ENEMY_DIAGONAL:
        return 'd';
    case ENEMY_ZIGZAG:
        return 'z';
    case ENEMY_FAST:
        return 'f';
    case ENEMY_MINI_BOSS:
        return 'M';
    case ENEMY_STAGE_BOSS:
        return 'B';
    }

    return 'v';
}

static void render_centered(int row, const char *text)
{
    int col = (GAME_WIDTH + 2 - (int)strlen(text)) / 2;
    if (col < 0) {
        col = 0;
    }
    mvprintw(row, col, "%s", text);
}

static void render_menu(void)
{
    render_clear_screen();

    attron(COLOR_PAIR(COLOR_PAIR_PLAYER));
    render_centered(4, "SUMMER CARNIVAL '92: RECCA");
    attroff(COLOR_PAIR(COLOR_PAIR_PLAYER));

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    render_centered(7, "Adaptacion en modo texto");
    render_centered(9, "W/A/S/D o flechas: mover");
    render_centered(10, "Espacio: disparar");
    render_centered(11, "Q: salir");
    render_centered(14, "Presione ENTER para iniciar");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));

    refresh();
}

static void render_game_over(const GameState *game)
{
    attron(COLOR_PAIR(COLOR_PAIR_ENEMY));
    render_centered(GAME_HEIGHT + 5, "GAME OVER");
    attroff(COLOR_PAIR(COLOR_PAIR_ENEMY));

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    mvprintw(GAME_HEIGHT + 6, 0, "Final Score: %06d  Level: %d  R: restart  Q: quit",
             game->player.score, game->level);
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));
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

    if (game->screen == GAME_SCREEN_MENU) {
        render_menu();
        return;
    }

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
            put_char(board,
                     game->enemies[i].position.x,
                     game->enemies[i].position.y,
                     enemy_char_for_type(game->enemies[i].type));
        }
    }

    for (int i = 0; i < MAX_EFFECTS; ++i) {
        if (game->effects[i].active) {
            put_char(board, game->effects[i].position.x, game->effects[i].position.y, '*');
        }
    }

    render_clear_screen();

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    mvprintw(0, 0, "Summer Carnival '92: Recca - texto");
    mvprintw(1, 0, "Score: %06d  Lives: %d  Level: %d  %s  Controls: W/A/S/D, arrows, Space, Q",
             game->player.score,
             game->player.lives,
             game->level,
             game->phase == LEVEL_PHASE_BOSS ? "BOSS" : "NORMAL");
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
    mvprintw(game->player.position.y + 3, game->player.position.x, "/A\\");
    attroff(COLOR_PAIR(COLOR_PAIR_PLAYER));

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));
    mvaddch(GAME_HEIGHT + 3, 0, '+');
    for (int x = 0; x < GAME_WIDTH; ++x) {
        mvaddch(GAME_HEIGHT + 3, x + 1, '-');
    }
    mvaddch(GAME_HEIGHT + 3, GAME_WIDTH + 1, '+');
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

    if (game->screen == GAME_SCREEN_GAME_OVER) {
        render_game_over(game);
    }

    refresh();
}
