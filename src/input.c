#include "input.h"

#include "types.h"

#include <ncursesw/curses.h>

int input_poll(void)
{
    int input = INPUT_NONE;
    int character = getch();

    switch (character) {
    case '\n':
    case '\r':
    case KEY_ENTER:
        input |= INPUT_START;
        break;
    case KEY_LEFT:
    case 'a':
    case 'A':
        input |= INPUT_LEFT;
        break;
    case KEY_RIGHT:
    case 'd':
    case 'D':
        input |= INPUT_RIGHT;
        break;
    case KEY_UP:
    case 'w':
    case 'W':
        input |= INPUT_UP;
        break;
    case KEY_DOWN:
    case 's':
    case 'S':
        input |= INPUT_DOWN;
        break;
    case ' ':
        input |= INPUT_FIRE;
        break;
    case 'r':
    case 'R':
        input |= INPUT_RESTART;
        break;
    case 'p':
    case 'P':
        input |= INPUT_PAUSE;
        break;
    case 'h':
    case 'H':
        input |= INPUT_HELP;
        break;
    case 'q':
    case 'Q':
        input |= INPUT_QUIT;
        break;
    default:
        break;
    }

    return input;
}
