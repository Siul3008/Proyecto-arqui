#include "render.h"

#include <curses.h>
#include <locale.h>
#include <string.h>

#include "config.h"
#include "enemy.h"

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
    case 'O':
        return COLOR_PAIR_PLAYER_SHOT;
    case 'v':
    case 'd':
    case 'z':
    case 'f':
    case 'M':
    case 'B':
    case '<':
    case '>':
    case '[':
    case ']':
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

static int bounded_enemy_center_x(const Enemy *enemy)
{
    int left_limit = enemy_min_center_x(enemy->type);
    int right_limit = enemy_max_center_x(enemy->type);

    if (enemy->position.x < left_limit) {
        return left_limit;
    }

    if (enemy->position.x > right_limit) {
        return right_limit;
    }

    return enemy->position.x;
}

static int bounded_enemy_y(const Enemy *enemy)
{
    if (enemy->position.y < 0) {
        return 0;
    }

    if (enemy->position.y >= GAME_HEIGHT) {
        return GAME_HEIGHT - 1;
    }

    return enemy->position.y;
}

static void put_enemy_on_board(char board[GAME_HEIGHT][GAME_WIDTH], const Enemy *enemy)
{
    int draw_y = bounded_enemy_y(enemy);

    if (enemy->type == ENEMY_MINI_BOSS) {
        const char *body = "<MMM>";
        int start_x = bounded_enemy_center_x(enemy) - enemy_hitbox_half_width(enemy->type);
        for (int i = 0; body[i] != '\0'; ++i) {
            put_char(board, start_x + i, draw_y, body[i]);
        }
        return;
    }

    if (enemy->type == ENEMY_STAGE_BOSS) {
        const char *body = "[BBBBB]";
        int start_x = bounded_enemy_center_x(enemy) - enemy_hitbox_half_width(enemy->type);
        for (int i = 0; body[i] != '\0'; ++i) {
            put_char(board, start_x + i, draw_y, body[i]);
        }
        return;
    }

    put_char(board, bounded_enemy_center_x(enemy), draw_y, enemy_char_for_type(enemy->type));
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
    mvprintw(GAME_HEIGHT + 6, 0, "Final Score: %06d  Rank: %d  Bosses: %d  R: restart  Q: quit",
             game->player.score, game->level, game->boss_count);
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));
}

static void render_boss_health_bar(const GameState *game)
{
    const Enemy *boss = enemies_find_boss(game->enemies, MAX_ENEMIES);

    if (boss == 0) {
        return;
    }

    int max_health = enemy_max_health(boss->type);
    int filled = (boss->health * 20) / max_health;

    attron(COLOR_PAIR(COLOR_PAIR_ENEMY));
    mvprintw(GAME_HEIGHT + 5, 0, "Boss HP: [");
    for (int i = 0; i < 20; ++i) {
        addch(i < filled ? '#' : '-');
    }
    printw("] %d/%d", boss->health, max_health);
    attroff(COLOR_PAIR(COLOR_PAIR_ENEMY));
}

static void render_boss_debug(const GameState *game)
{
    const Enemy *boss = enemies_find_boss(game->enemies, MAX_ENEMIES);

    if (boss == 0) {
        return;
    }

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    mvprintw(GAME_HEIGHT + 6, 0,
             "Boss debug: type=%c x=%d y=%d vx=%d left=%d right=%d limits=%d..%d",
             enemy_char_for_type(boss->type),
             boss->position.x,
             boss->position.y,
             boss->velocity.x,
             enemy_left(boss),
             enemy_right(boss),
             enemy_min_center_x(boss->type),
             enemy_max_center_x(boss->type));
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
            put_enemy_on_board(board, &game->enemies[i]);
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
    int charge_percent = (game->player.charge_frames * 100) / PLAYER_CHARGE_MAX;
    mvprintw(1, 0, "Score: %06d  Lives: %d  Rank: %d  Charge: %3d%%  %s  Next Boss: %d",
             game->player.score,
             game->player.lives,
             game->level,
             charge_percent,
             game->phase == LEVEL_PHASE_BOSS ? "BOSS" : "NORMAL",
             game->next_boss_score);
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

    if (game->player.invulnerable_timer == 0 ||
        (game->player.invulnerable_timer / 5) % 2 == 0) {
        attron(COLOR_PAIR(COLOR_PAIR_PLAYER));
        mvprintw(game->player.position.y + 3, game->player.position.x, "/A\\");
        attroff(COLOR_PAIR(COLOR_PAIR_PLAYER));
    }

    if (game->player.charge_frames > 0 && game->player.position.y > 0) {
        int charge_color = game->player.charge_frames >= PLAYER_CHARGE_RELEASE_MIN ?
                           COLOR_PAIR_PLAYER_SHOT :
                           COLOR_PAIR_PLAYER;
        attron(COLOR_PAIR(charge_color));
        mvaddch(game->player.position.y + 2, game->player.position.x + 1, 'O');
        attroff(COLOR_PAIR(charge_color));
    }

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));
    mvaddch(GAME_HEIGHT + 3, 0, '+');
    for (int x = 0; x < GAME_WIDTH; ++x) {
        mvaddch(GAME_HEIGHT + 3, x + 1, '-');
    }
    mvaddch(GAME_HEIGHT + 3, GAME_WIDTH + 1, '+');
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

    if (game->screen == GAME_SCREEN_GAME_OVER) {
        render_game_over(game);
    } else if (game->phase == LEVEL_PHASE_BOSS) {
        render_boss_health_bar(game);
        render_boss_debug(game);
    }

    refresh();
}
