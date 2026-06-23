#include "render.h"

#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

#include "arm_gas.h"
#include "config.h"
#include "enemy.h"
#include "player.h"

/************************************************************************
* Estructura: 
    enum
* Descripción: 
    Útil más adelante en el código para manejar colores de los elementos
        en pantalla
*************************************************************************/
enum {
    COLOR_PAIR_PLAYER = 1,
    COLOR_PAIR_PLAYER_SHOT,
    COLOR_PAIR_ENEMY,
    COLOR_PAIR_ENEMY_SHOT,
    COLOR_PAIR_BORDER,
    COLOR_PAIR_TEXT,
    COLOR_PAIR_POWERUP,
    COLOR_PAIR_STAR,
    COLOR_PAIR_HIGHLIGHT
};

/************************************************************************
* Función: 
    fill_board
* Descripción: 
    Deja en blanco el área de juego
* Entradas: 
    Matriz de caracteres con las siguientes dimensiones
        Altura = Altura área de juego (Definido en config.c)
        Ancho = Ancho área de juego (Definido en config.c)
* Salidas: 
    Ninguna
*************************************************************************/
static void fill_board(char board[GAME_HEIGHT][GAME_WIDTH])
{
#if defined(RECCA_USE_ARM_GAS)
    recca_fill_board_gas(&board[0][0], GAME_HEIGHT * GAME_WIDTH, ' ');
#else
    //Mientras y sea menor que la altura del área de juego (Llena filas completas)
    for (int y = 0; y < GAME_HEIGHT; ++y) {
        //Mientras y sea menor que el ancho del área de juego (Llena casillas de columna)
        for (int x = 0; x < GAME_WIDTH; ++x) {
            board[y][x] = ' ';  //Coloca un caracter vacio o de espacio en la casilla con las coordenadas [y][x]
        }
    }
#endif
}

/************************************************************************
* Función: 
    put_char
* Descripción: 
    Coloca el caracter proporcionado en la coordenadas indicadas
* Entradas: 
    Matriz de caracteres con las siguientes dimensiones
        Altura = Altura área de juego (Definido en config.c)
        Ancho = Ancho área de juego (Definido en config.c)
    Número entero que indica coordenada x
    Número entero que indica coordenada y
    Caracter a colocar
* Salidas: 
    Ninguna
*************************************************************************/
static void put_char(char board[GAME_HEIGHT][GAME_WIDTH], int x, int y, char value)
{
    /*Si:
        x es mayor o igual a cero
        x es menor que el ancho total del area de juego 
        y es mayor o igual a cero
        y es menor que la altura del area de juego */
    if (x >= 0 && x < GAME_WIDTH && y >= 0 && y < GAME_HEIGHT) {
        board[y][x] = value;    //Coloca el valor indicado en las coordenadas proporcionadas
    }
}

/************************************************************************
* Función: 
    put_starfield_on_board
* Descripción: 
    Coloca en pantalla el efecto visual correspondiente a un campo de
        estrellas que va pasando, esto ayuda al jugador que este juego 
        tiene un ambiente espacial
* Entradas: 
    Matriz de caracteres con las siguientes dimensiones
        Altura = Altura área de juego (Definido en config.c)
        Ancho = Ancho área de juego (Definido en config.c)
    Número entero que indica número de frame actual
* Salidas: 
    Ninguna
*************************************************************************/
static void put_starfield_on_board(char board[GAME_HEIGHT][GAME_WIDTH], int frame)
{
    //Inicializa valor de cuando debe "scrollear", avanzar o desplazarse el fondo al dividir el número de frame
    //  entre el intervalo de desplazamiento del fondo (valor definido en config.h)
    int scroll = frame / STARFIELD_SCROLL_INTERVAL;

    //Mientras i sea menor a la cantidad de puntos que deben aparecer por campo de estrellas (valor definido en config.h)
    for (int i = 0; i < STARFIELD_DOT_COUNT; ++i) {
        //La coordenada x del punto a colocar será calculoada de la siguiente forma:
        //  1. Multiplica el índice actual por 17 (Número primo)
        //  2. Divide el índice actual entre 3 (Número primo)
        //  3. Suma esos dos resultados
        //  4. Toma el residuo de dividir el resultado de esa suma entre el ancho (en caracteres) del juego
        int x = (i * 17 + i / 3) % GAME_WIDTH;

        //La coordenada x del punto a colocar será calculoada de la siguiente forma:
        //  1. Multiplica el índice actual por 7 (Número primo)
        //  2. Suma ese resultado con el valor de scroll/desplazamiento calculado previamente
        //  3. Toma el residuo de dividir el resultado de esa suma entre la altura (en caracteres) del juego
        int y = (i * 7 + scroll) % GAME_HEIGHT;

        //Es importante destacar que, se usan valores específicos como 17 o 7 ya que costará mucho obtener una división exacta, 
        //  lo que propicia la aparición de patrones, si bien no completamente aleatorios, lo suficientemente aleatorios como para
        //  que no se note un patrón de movimiento definido

        /*Para determinar cual es el caracter que se debe usar para representar una estrella, se hace lo siguiente:
            1. Suma el índice actual con el valor de scroll/desplazamiento calculado previamente
            2. Toma el residuo de dividir este resultado entre 4
        Si este residuo es igual a cero, se usará ":", de lo contrario, "."*/
        char value = ((i + scroll) % 4 == 0) ? ':' : '.';

        //Coloca el caracter obtenido en las coordenadas calculadas previamente
        put_char(board, x, y, value);
    }
}

/************************************************************************
* Función: 
    color_for_char
* Descripción: 
    Asigna un color al caracter ingresado, tomando en cuenta lo que 
        se quiere representar con este
* Entradas: 
    Matriz de caracteres con las siguientes dimensiones
        Altura = Altura área de juego (Definido en config.c)
        Ancho = Ancho área de juego (Definido en config.c)
    Número entero que indica número de frame actual
* Salidas: 
    Entero que indica que valor de color usar
*************************************************************************/
static int color_for_char(char value)
{
#if defined(RECCA_USE_ARM_GAS)
    return recca_color_for_char_gas((unsigned char)value);
#else
    switch (value) {
    //---------- Proyectiles del jugador ----------
    case '|':
    case 'O':
    case '@':
        return COLOR_PAIR_PLAYER_SHOT;
    case 'F':
    case 'S':
    case 'L':
    case 'T':
    case 'H':
    case 'D':
        return COLOR_PAIR_POWERUP;

    //---------- Naves enemigas ----------
    case 'v':
    case 'd':
    case 'z':
    case 'f':
    case 'w':
    case 'x':
    case 'M':
    case 'B':
    case '<':
    case '>':
    case '[':
    case ']':
        return COLOR_PAIR_ENEMY;

    //---------- Proyectil enemigo ----------
    case 'o':
        return COLOR_PAIR_ENEMY_SHOT;

    //---------- Proyectil del jugador ----------
    case '*':
        return COLOR_PAIR_PLAYER_SHOT;

    //---------- Fondo animado ----------
    case '.':
    case ':':
        return COLOR_PAIR_STAR;

    //---------- Nave del jugador ----------        
    case 'A':
        return COLOR_PAIR_PLAYER;
    //Si no es ninguno de los anteriores
    default:
        return COLOR_PAIR_TEXT;
    }
#endif
}

/************************************************************************
* Función: 
    enemy_char_for_type
* Descripción: 
    Obtiene el caracter correspondiente al tipo de enemigo que se 
        desea representar
* Entradas: 
    Tipo de enemigo EnemyType
* Salidas: 
    Caracter a usar según tipo de enemigo
*************************************************************************/
/* GAS path helpers for repeated board population work. */
#if defined(RECCA_USE_ARM_GAS)
static void put_projectiles_on_board(char board[GAME_HEIGHT][GAME_WIDTH],
                                     const Projectile projectiles[],
                                     int count,
                                     char value)
{
#if defined(RECCA_USE_ARM_GAS)
    recca_put_projectiles_on_board_gas(&board[0][0], projectiles, count, value);
#else
    for (int i = 0; i < count; ++i) {
        if (projectiles[i].active) {
            put_char(board, projectiles[i].position.x, projectiles[i].position.y, value);
        }
    }
#endif
}

static void put_effects_on_board(char board[GAME_HEIGHT][GAME_WIDTH],
                                 const Effect effects[],
                                 int count,
                                 char value)
{
#if defined(RECCA_USE_ARM_GAS)
    recca_put_effects_on_board_gas(&board[0][0], effects, count, value);
#else
    for (int i = 0; i < count; ++i) {
        if (effects[i].active) {
            put_char(board, effects[i].position.x, effects[i].position.y, value);
        }
    }
#endif
}
#endif

/* Returns the display character used for each enemy type. */
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
    case ENEMY_SWEEP:
        return 'w';
    case ENEMY_DIVE:
        return 'x';
    case ENEMY_MINI_BOSS:
        return 'M';
    case ENEMY_STAGE_BOSS:
        return 'B';
    }

    return 'v';
}

/************************************************************************
* Función: 
    weapon_char_for_type
* Descripción: 
    Obtiene el caracter correspondiente al tipo de arma que se 
        desea representar, esto se va a usar cuando un enemigo las suelta
        al haberlo derrotado
* Entradas: 
    Tipo de arma WeaponType
* Salidas: 
    Caracter a usar según tipo de arma
*************************************************************************/
static char weapon_char_for_type(WeaponType weapon)
{
    switch (weapon) {
    case WEAPON_FRONT:
        return 'F';
    case WEAPON_SPREAD:
        return 'S';
    case WEAPON_LASER:
        return 'L';
    case WEAPON_DOUBLE:
        return 'T';
    case WEAPON_SIDE:
        return 'H';
    }

    //En caso de que el switch no lo haya atrapado, se asumirá que es tipo FRONT
    return 'F';
}

/************************************************************************
* Función: 
    powerup_char_for_type
* Descripción: 
    Obtiene el caracter correspondiente al tipo de powerup que se 
        desea representar, esto se va a usar cuando un enemigo las suelta
        al haberlo derrotado
* Entradas: 
    Puntero a instancia de objeto PowerUp
* Salidas: 
    Ninguna
*************************************************************************/
static char powerup_char_for_type(const PowerUp *powerup)
{
    //Si el powerup es de tipo dron
    if (powerup->type == POWERUP_DRONE) {
        return 'D';
    }
    
    //De lo contrario, es un arma, por tanto, llamará a la función usada para armas y retornará ese resultado
    return weapon_char_for_type(powerup->weapon);
}

/************************************************************************
* Función: 
    weapon_name_for_type
* Descripción: 
    Obtiene el nombre del arma en uso, para así desplegarlo en pantalla
* Entradas: 
    Tipo de arma WeaponType
* Salidas: 
    Puntero a cadena de caracteres que contiene el nombre del arma
*************************************************************************/
static const char *weapon_name_for_type(WeaponType weapon)
{
    switch (weapon) {
    case WEAPON_FRONT:
        return "FRONT";
    case WEAPON_SPREAD:
        return "SPREAD";
    case WEAPON_LASER:
        return "LASER";
    case WEAPON_DOUBLE:
        return "DOUBLE";
    case WEAPON_SIDE:
        return "SIDE";
    }

    //En caso de que el switch no lo haya atrapado, se asumirá que es tipo FRONT
    return "FRONT";
}

/************************************************************************
* Función: 
    timer_seconds
* Descripción: 
    Calcula el segundo en el que se encuentra el juego
* Entradas: 
    Entero que indica la cantidad de frames que han pasado desde el 
        inicio de la partida
* Salidas: 
    Entero que indica, en segundos, el tiempo que ha estado corriendo el
        programa/juego
*************************************************************************/
static int timer_seconds(int frames)
{   
    //Se calcula cuantos frames hay por segundo, esto al dividir mil entre el intervalo (en milisegundos) que hay entre cada frame (definido en config.h)
    int frames_per_second = 1000 / FRAME_DELAY_MS;

    //Si el resultado de esto es menor o igual a cero, significa que no ha transcurrido nada de tiempo
    if (frames_per_second <= 0) {
        //Retorna cero
        return 0;
    }

    //De lo contrario, retorna el resultado de dividir el número de frames que ha transcurrido entre el número de frames por segundo, calculado anteriormente
    return frames / frames_per_second;
}

/************************************************************************
* Función: 
    bounded_enemy_center_x
* Descripción: 
    Verifica que la coordenada x en la que se encuentra el enemigo, 
        se encuentre dentro de los límites permitidos para su tipo
* Entradas: 
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Entero que indica la coordenada x central que debe tener el enemigo
*************************************************************************/
static int bounded_enemy_center_x(const Enemy *enemy)
{
    //Calcula los límites laterales (con respecto al área de juego) del enemigom, tomando en cuenta el tipo de este
    int left_limit = enemy_min_center_x(enemy->type);
    int right_limit = enemy_max_center_x(enemy->type);

    //Si la posición x actual del enemigo es menor que el valor del límite izquierdo
    if (enemy->position.x < left_limit) {
        return left_limit;  //Retorna el límite izquierdo
    }

    //Si la posición x actual del enemigo es mayor que el valor del límite derecho 
    if (enemy->position.x > right_limit) {
        return right_limit; //Retorna el límite derecho
    }

    //De lo contrario, significa que está dentro de los límites, por tanto:
    return enemy->position.x;   //Devuelve el valor de posición que este tenía
}

/************************************************************************
* Función: 
    bounded_enemy_y
* Descripción: 
    Verifica que la coordenada y en la que se encuentra el enemigo, se 
        encuentre dentro de los límites dle área de juego
* Entradas: 
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Entero que indica la coordenada y que debe tener el enemigo
*************************************************************************/
static int bounded_enemy_y(const Enemy *enemy)
{
    //Si la posición y actual es menor a cero
    if (enemy->position.y < 0) {
        return 0;   //Retorna cero
    }

    //Si la posición y actual del enemigo es mayor o igual que la altura máxima del área de juego
    if (enemy->position.y >= GAME_HEIGHT) {
        return GAME_HEIGHT - 1; //Devuelve el valor de la altura máxima del área de juego, decrementado una vez
    }

    //De lo contrario, significa que está dentro de los límites, por tanto:
    return enemy->position.y;   //Devuelve el valor de posición que este tenía
}

/************************************************************************
* Función: 
    put_enemy_on_board
* Descripción: 
    Coloca al enemigo proporcionado en las coordenadas que este
        especifica (siempre y cuando sean correctas) tomando en cuenta 
        su tipo 
* Entradas: 
    Matriz de caracteres con las siguientes dimensiones
        Altura = Altura área de juego (Definido en config.c)
        Ancho = Ancho área de juego (Definido en config.c)
    Puntero a instancia de objeto Enemy (enemigo)
* Salidas: 
    Ninguna
*************************************************************************/
static void put_enemy_on_board(char board[GAME_HEIGHT][GAME_WIDTH], const Enemy *enemy)
{
    //Obtiene la coordenada y a la que se debe de dibujar en pantalla por primera vez al enemigo
    int draw_y = bounded_enemy_y(enemy);

    //Si el enemigo es un minijefe
    if (enemy->type == ENEMY_MINI_BOSS) {
        const char *body = "<MMM>"; //Define la cadena de caracteres que lo representará

        /*Obtiene la coordenada x a la que se debe de dibujar en pantalla por primera vez al enemigo, esto lo hace de la siguiente forma:
            1. Obtiene un valor de x dentro de los límites del área de juego
            2. Le resta la mitad de su hitbox para así, obtener la coordenada x de su costado lateral izquierdo, para empezarlo a dibujar desde ese punto*/
        int start_x = bounded_enemy_center_x(enemy) - enemy_hitbox_half_width(enemy->type); 

        //Mientras el caracter en el indice i no sea un caracter nulo
        for (int i = 0; body[i] != '\0'; ++i) {
            //Llama a función para que se dibuje el caracter actual de la cadena en las coordenadas que se indica
            //  (Por cada caracter que pase, la coordenada x va a ir incrementando, es decir, para representar un enemigo en pantalla, se puede ver como que 
            //      si lo estuviera imprimiendo caracter por caracter)
            put_char(board, start_x + i, draw_y, body[i]);
        }
        return;
    }

    //Si el enemigo es un jefe
    if (enemy->type == ENEMY_STAGE_BOSS) {
        const char *body = "[BBBBB]";   //Define la cadena de caracteres que lo representará

        /*Obtiene la coordenada x a la que se debe de dibujar en pantalla por primera vez al enemigo, esto lo hace de la siguiente forma:
            1. Obtiene un valor de x dentro de los límites del área de juego
            2. Le resta la mitad de su hitbox para así, obtener la coordenada x de su costado lateral izquierdo, para empezarlo a dibujar desde ese punto*/
        int start_x = bounded_enemy_center_x(enemy) - enemy_hitbox_half_width(enemy->type);


        //Mientras el caracter en el indice i no sea un caracter nulo
        for (int i = 0; body[i] != '\0'; ++i) {
            //Llama a función para que se dibuje el caracter actual de la cadena en las coordenadas que se indica
            //  (Por cada caracter que pase, la coordenada x va a ir incrementando, es decir, para representar un enemigo en pantalla, se puede ver como que 
            //      si lo estuviera imprimiendo caracter por caracter)
            put_char(board, start_x + i, draw_y, body[i]);
        }
        return;
    }

    //De lo contrario, como los enemigos normales están formados por un solo caracter, llama para que este sea impreso de inmediato
    put_char(board, bounded_enemy_center_x(enemy), draw_y, enemy_char_for_type(enemy->type));
}

/************************************************************************
* Función: 
    render_centered
* Descripción: 
    Se asegura de renderizar la cadena de texto ingresada, en el centro 
        del área de juego
* Entradas: 
    Número entero que indica la fila en la que se quiere desplegar la
        cadena de caracteres
    Puntero a cadena de caracteres que se desea desplegar en pantalla
* Salidas: 
    Ninguna
*************************************************************************/
static void render_centered(int row, const char *text)
{
    /*Calcula la columna en la que debe iniciar el renderizado del texto, lo hace de la siguiente forma:
        1. Divide entre dos el largo total de la cadena de caracteres
        2. Toma el ancho total del juego (definido en config.h) y le suma 2
        3. Le resta el resultado obtenido en el punto 1., al resultado obtenido en el punto 2. */
    int col = (GAME_WIDTH + 2 - (int)strlen(text)) / 2;

    //Si el resultado de esta operación es menor a cero
    if (col < 0) {
        col = 0;    //Establece este valor en cero
    }

    /*Le pasa a la función de impresión en consola los siguientes parámetros
        1. Coordenada x desde la cual se desea empezar a imprimir
        2. Coordenada y desde la cual se desea empezar a imprimir
        3. Formato en el que se desea imprimir (En este caso, imprime los caracteres de la cadena texto hasta encontrar un caracter nulo)
        4. Puntero a cadena de caracteres que se desea imprimir*/
    mvprintw(row, col, "%s", text);
}

/************************************************************************
* Función: 
    render_centered_color
* Descripción: 
    Se asegura de renderizar la cadena de texto ingresada, en el centro 
        del área de juego usando el código de color proporcionado
* Entradas: 
    Número entero que indica la fila en la que se quiere desplegar la
        cadena de caracteres
    Puntero a cadena de caracteres que se desea desplegar en pantalla
    Número entero que indica código del par de color a utilizar
* Salidas: 
    Ninguna
*************************************************************************/
static void render_centered_color(int row, const char *text, int color_pair)
{   
    //Es importante destacar la forma en la que funcionan los atributos en ncurses, ya que, de la manera que se usa en este proyecto, para cambiar atributos como el color, se hace uso de las funciones attron y attroff
    //  attron lo que hace es, que todo código que esté después de haberse usado la función, tenga el atributo especificado en esta
    //  attroff, al contrario, remueve el atributo especificado

    //Aplica atributo de color y llama a función de renderizado en el centro de la pantalla

    attron(COLOR_PAIR(color_pair));     //A todo lo que esté debajo, le otorga el atributo de color proporcionado
    render_centered(row, text);         //La línea de texto que se renderice aquí, va a tener a COLOR_PAIR_PLAYER como atributo de color
    attroff(COLOR_PAIR(color_pair));    //A todo lo que esté debajo, se le quitará el atributo de color proporcionado, es decir, ya no tendrá ese atributo, a no ser que se le vuelva a dar con otro attron
}

/************************************************************************
* Función: 
    render_centered_rule
* Descripción: 
    Se asegura de renderizar en pantalla una línea de margen en el centro
        de la pantalla
* Entradas: 
    Número entero que indica la fila en la que se quiere desplegar el 
        margen 
* Salidas: 
    Ninguna
*************************************************************************/
static void render_centered_rule(int row)
{
    int width = GAME_WIDTH - 12;
    int col = 6;

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));
    for (int i = 0; i < width; ++i) {
        mvaddch(row, col + i, '-');
    }
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));
}

// ------------------------------------- RENDERIZADO DE PANTALLAS DEL JUEGO -------------------------------------

/************************************************************************
* Función: 
    render_init
* Descripción: 
    Encargada de inicializar el motor de renderizado
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void render_init(void)
{
    //---------------- CONFIGURACIÓN CONSOLA ----------------

    setlocale(LC_ALL, "");  //Configura la consola para que admita caracteres especiales
    initscr();  //Inicializa la biblioteca de ncurses para poder hacer uso de sus funciones
    cbreak();   //Deshabilita el tener que esperar un enter para recibir una entrada, es decir, apenas se presiona una tecla, se registrará esa entrada
    noecho();   //Deshabilita la impresión automática de la entrada del usuario, es decir, no van a aparecer caracteres extra cada vez que se presione una tecla
    keypad(stdscr, TRUE);   //Permite que teclas de caracteres especiales, como las de las flechas, puedan ser leídas y usadas por el programa
    nodelay(stdscr, TRUE);  //Establece que la función, getch(), la misma usada para procesar las entradas del usuario, pueda simplemente salir de la función si no 
                            //  detecta que alguna tecla ha sido presionada, ya que, de lo contrario, congelaría todo hasta obtener una entrada por parte del usuario
    curs_set(0);    //Oculta el cursor del mouse

    //Verifica si la terminal en la que está corriendo el programa es capaz de mostrar colores
    if (has_colors()) {
        //Si la terminal es capaz de mostrar colores, inicializa el componente de ncurses encargado del manejo de color
        start_color();

        /*Además, configura los colores que serán usados en el juego, esto con los siguientes argumentos:
            1. Número de par de color a cambiar (He aquí, la utilidad del enum al inicio del archivo)
            2. Color del primer plano (el color del caracter por ejemplo)
            3. Color del segundo plano (el color detrás del caracter, o el resaltado de este)*/
        init_pair(COLOR_PAIR_PLAYER, COLOR_CYAN, COLOR_BLACK);
        init_pair(COLOR_PAIR_PLAYER_SHOT, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_PAIR_ENEMY, COLOR_RED, COLOR_BLACK);
        init_pair(COLOR_PAIR_ENEMY_SHOT, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(COLOR_PAIR_BORDER, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_PAIR_TEXT, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_PAIR_POWERUP, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_PAIR_STAR, COLOR_BLUE, COLOR_BLACK);
        init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_YELLOW, COLOR_BLACK);
    }
}

/************************************************************************
* Función: 
    render_menu
* Descripción: 
    Encargada de renderizar la pantalla de menú principal
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
static void render_menu(void)
{
    //Limpia la pantalla para no imprimir sobre otros caracteres que pueden haber quedado, es decir, para no imprimir sobre caracteres residuales

    render_clear_screen();

    //Renderiza linea de margen en el centro de la pantalla
    render_centered_rule(2);

    //Renderiza en el centro de la línea especificada al pasar los siguientes parámetros a la función render_centered_color
    //  1. Número de línea en la que se desea renderizar el texto
    //  2. Cadena de texto a renderizar
    //  3. Color con el que se desea renderizar el texto
    render_centered_color(4, "SUMMER CARNIVAL '92: RECCA", COLOR_PAIR_PLAYER);
    render_centered_color(5, "TEXT MODE SHOOTER", COLOR_PAIR_HIGHLIGHT);
    render_centered_rule(7);

    render_centered_color(9, "      /A\\      ", COLOR_PAIR_PLAYER);
    render_centered_color(10, "   @   |   @   ", COLOR_PAIR_PLAYER_SHOT);
    render_centered_color(11, " .  .  :  .  . ", COLOR_PAIR_STAR);
    attron(COLOR_PAIR(COLOR_PAIR_TEXT));    //Establece color por defecto (Blanco)      
    //Renderiza cadenas de texto en el medio de la línea indicada
    render_centered(14, "W/A/S/D or arrows: move");
    render_centered(15, "Space: fire / release charged bomb");
    render_centered(16, "H: help    Q: quit");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));

    render_centered_color(19, "PRESS ENTER TO START", COLOR_PAIR_HIGHLIGHT);


    refresh();  //Hace un refresco de pantalla
    
    return; //Sale de la función
}

/************************************************************************
* Función: 
    render_help
* Descripción: 
    Encargada de renderizar la pantalla de menú de ayuda
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
static void render_help(void)
{
    render_clear_screen();  //Limpia la pantalla para no imprimir sobre otros caracteres que pueden haber quedado, es decir, para no imprimir sobre caracteres residuales

    //Renderiza en el centro de la línea especificada al pasar los siguientes parámetros a la función render_centered_color
    //  1. Número de línea en la que se desea renderizar el texto
    //  2. Cadena de texto a renderizar
    //  3. Color con el que se desea renderizar el texto

    //En caso de hacer uso de render_centered, solo se pasan los parámetros 1. y 2.

    render_centered_color(3, "HELP", COLOR_PAIR_PLAYER);
    render_centered_rule(5);

    render_centered_color(7, "MOVEMENT", COLOR_PAIR_HIGHLIGHT);
    render_centered(8, "W/A/S/D or arrows");

    render_centered_color(10, "COMBAT", COLOR_PAIR_HIGHLIGHT);
    render_centered(11, "Space: fire");
    render_centered(12, "Release Space: charge shield");
    render_centered(13, "Press Space after charge: bomb");

    render_centered_color(15, "ITEMS", COLOR_PAIR_HIGHLIGHT);
    render_centered(16, "F/S/L/T/H: temporary weapons");
    render_centered(17, "D: temporary drones");

    render_centered_color(20, "P: pause   R: restart   Q: quit", COLOR_PAIR_TEXT);
    render_centered_color(22, "H, P or ENTER: return", COLOR_PAIR_HIGHLIGHT);

    refresh();  //Hace un refresco de pantalla
    
    return; //Sale de la función
}

/************************************************************************
* Función: 
    render_game_over
* Descripción: 
    Encargada de renderizar la pantalla de fin del juego (GAME OVER)
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
static void render_game_over(const GameState *game)
{
    char line[64]; //Inicizaliza buffer de 64 caracteres

    
    render_centered_rule(GAME_HEIGHT / 2 - 2);  //Renderiza margen
    //Renderiza texto de GAME OVER en medio del área de juego con texto en color rojo
    render_centered_color(GAME_HEIGHT / 2, "GAME OVER", COLOR_PAIR_ENEMY);

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));    //Establece color por defecto (Blanco)
    //Renderiza cadena de texto en el medio del área de juego
    render_centered(GAME_HEIGHT / 2 + 2, "RUN SUMMARY");

    /*Imprime puntaje del jugador en buffer "line" con el siguiente formato:
        Score : (Dato decimal de 6 dígitos que indica el puntaje total, si el número no alcanza a cubrir todos los dígitos, se rellenará con ceros a la izquierda)*/
    snprintf(line, sizeof(line), "Score : %06d", game->player.score);
    //Renderiza buffer dos líneas más abajo del texto de encabezado
    render_centered(GAME_HEIGHT / 2 + 4, line);

    /*Imprime rango/nivel del jugador en buffer "line" con el siguiente formato:
        Rank : (Dato decimal que indica rango/nivel alcanzado)*/
    snprintf(line, sizeof(line), "Rank  : %d", game->level);
    //Renderiza buffer una línea más abajo de la línea anterior
    render_centered(GAME_HEIGHT / 2 + 5, line);

    /*Imprime cantidad de jefes vencidos por jugador en buffer "line" con el siguiente formato:
        Bosses : (Dato decimal que indica cantidad de jefes vencidos)*/
    snprintf(line, sizeof(line), "Bosses: %d", game->boss_count);
    //Renderiza buffer una línea más abajo de la línea anterior
    render_centered(GAME_HEIGHT / 2 + 6, line);
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));

    //Imprime encabezado de mejores puntuaciones dos líneas más abajo de la línea anterior en color resaltado
    render_centered_color(GAME_HEIGHT / 2 + 8, "HIGH SCORES", COLOR_PAIR_HIGHLIGHT);

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    //Mientras i sea menor a la cantidad máxima de puntuaciones a mostrar (dato definido en config.h)
    for (int i = 0; i < MAX_HIGH_SCORES; ++i) {
        //Toma puntero a objeto HighScoreEntry guardado en el índice actual del arreglo de mejores puntuaciones guardado en el objeto Game
        const HighScoreEntry *entry = &game->high_scores[i];

        //Si el puntaje del entry es mayor cero
        if (entry->score > 0) {
            /*Imprime cantidad de jefes vencidos por jugador en buffer "line" con el siguiente formato:
                (Número en el ranking) (Nombre del jugador) (Puntaje final del jugador) (Rango/nivel alcanzado por el jugador) (Número de jefes derrotados por el jugador)*/
            snprintf(line,
                     sizeof(line),
                     "%d. %-10s %06d R:%d B:%d",
                     i + 1,
                     entry->name,
                     entry->score,
                     entry->rank,
                     entry->bosses);
        } else {
            //De lo contrario, simplemente imprimirá el número de ranking disponible con los datos en blanco de la siguiente forma:
            //  (Número en el ranking) ---------- ------
            snprintf(line, sizeof(line), "%d. ---------- ------", i + 1);
        }
        //Renderiza el buffer "line" una línea más abajo de la anterior
        render_centered(GAME_HEIGHT / 2 + 10 + i, line);
    }
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));

    //Renderiza en la parte inferior del área de juego las acciones disponibles en color resaltado
    render_centered_color(GAME_HEIGHT / 2 + 16, "R: restart  H: help  Q: quit", COLOR_PAIR_HIGHLIGHT);

    return; //Sale de la función
}

/************************************************************************
* Función: 
    render_name_entry
* Descripción: 
    Encargada de renderizar la pantalla de ingreso de nombre por parte
        del jugador
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
static void render_name_entry(const GameState *game)
{
    char line[64]; //Inicizaliza buffer de 64 caracteres

    render_clear_screen();  //Limpia la pantalla para no imprimir sobre otros caracteres que pueden haber quedado, es decir, para no imprimir sobre caracteres residuales

    render_centered_rule(3);    //Renderiza margen en la tercera fila
    //Renderiza texto "NEW HIGH SCORE" en medio de la fila 5 en color Cyan
    render_centered_color(5, "NEW HIGH SCORE", COLOR_PAIR_PLAYER);

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));//Establece color por defecto (Blanco)
    /*Imprime puntaje del jugador en buffer "line" con el siguiente formato:
        Score : (Dato decimal de 6 dígitos que indica el puntaje total, si el número no alcanza a cubrir todos los dígitos, se rellenará con ceros a la izquierda)*/
    snprintf(line, sizeof(line), "Score : %06d", game->player.score);
    //Renderiza buffer tres líneas más abajo del texto de encabezado
    render_centered(8, line);
    
    /*Imprime rango/nivel del jugador en buffer "line" con el siguiente formato:
        Rank : (Dato decimal que indica rango/nivel alcanzado)*/
    snprintf(line, sizeof(line), "Rank  : %d", game->level);
    //Renderiza buffer una línea más abajo de la línea anterior
    render_centered(9, line);

    /*Imprime cantidad de jefes vencidos por jugador en buffer "line" con el siguiente formato:
        Bosses : (Dato decimal que indica cantidad de jefes vencidos)*/
    snprintf(line, sizeof(line), "Bosses: %d", game->boss_count);
    //Renderiza buffer una línea más abajo de la línea anterior
    render_centered(10, line);
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));

    /*Imprime línea destinada al ingreso del nombre del jugador en buffer "line" con el siguiente formato:
        Name: (Lo que ha ingresado el jugador hasta el momento, si esto es menor a 10 caracteres, los espacios libres serán rellenados por _, 
                de lo contrario no escribirá nada más, indicandole al usuario que se ha quedado sin caracteres disponibles)*/
    snprintf(line,
             sizeof(line),
             "Name  : %-10s%s",
             game->name_input,
             game->name_length < PLAYER_NAME_MAX_LENGTH ? "_" : "");
    render_centered_color(13, line, COLOR_PAIR_HIGHLIGHT);

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));
    //Renderiza requerimientos de entrada tres líneas más abajo de la línea anterior
    render_centered(16, "Letters and numbers only, max 10");
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));
    //Renderiza teclas de control dos líneas más abajo de la línea anterior
    render_centered_color(18, "ENTER: save  BACKSPACE: delete", COLOR_PAIR_TEXT);

    refresh();  //Hace un refresco de pantalla
    return;
}

/************************************************************************
* Función: 
    render_pause_overlay
* Descripción: 
    Encargada de renderizar la pantalla de pausa
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
static void render_pause_overlay(void)
{
    render_centered_rule(GAME_HEIGHT / 2);  //Renderiza un margen en el centro del area de juego
    //Renderiza en pantalla el mensaje de pausa
    render_centered_color(GAME_HEIGHT / 2 + 2, "PAUSED", COLOR_PAIR_HIGHLIGHT);
    //Renderiza en pantalla las acciones disponibles y las teclas correspondientes en color blanco
    render_centered_color(GAME_HEIGHT / 2 + 4, "P: resume  Q: quit", COLOR_PAIR_TEXT);
    return;
}


/************************************************************************
* Función: 
    render_boss_healthbar
* Descripción: 
    Encargada de renderizar la barra de vida del jefe en la parte inferior
        del área de juego
* Entradas: 
    Puntero a instancia de objeto tipo GAME (juego)
* Salidas: 
    Ninguna
*************************************************************************/
static void render_boss_health_bar(const GameState *game)
{   
    //Obtiene un puntero al objeto kefe activo en el juego
    const Enemy *boss = enemies_find_boss(game->enemies, MAX_ENEMIES);
    
    //Si no hay ningún jefe activo
    if (boss == 0) {
        return; //Sale de la función
    }

    //De lo contrario 
    int max_health = enemy_max_health(boss->type); //Obtiene la cantidad de vida máxima del jefe
    /*Además, determina cuanto porcentaje de la barra de vida del jefe debe estar llena, esto lo hace de la siguiente forma 
        1. Toma la cantidad de vida que tiene le jefe actualmente y la multiplica por 20
        2. Divide el resultado de la multiplicación entre la cantidad máxima de vida que puede tener el jefe*/
    int filled = (boss->health * 20) / max_health;  

    attron(COLOR_PAIR(COLOR_PAIR_ENEMY));   //Establece color a COLOR_PAIR_ENEMY (Rojo)
    //Imprime en pantalla la parte inicial de la barra de vida
    mvprintw(GAME_HEIGHT + 5, 0, "Boss HP: ["); 
    //Mientras i sea menor a 20
    for (int i = 0; i < 20; ++i) {
        //Si i es menor al entero que indica cuanto debe estar llena la barra de vida, imprime el caracter '#', de lo contrario, imprimirá '-'
        addch(i < filled ? '#' : '-');
    }
    //Imprime en pantalla la parte final de la barra de vida
    printw("] %d/%d", boss->health, max_health);
    attroff(COLOR_PAIR(COLOR_PAIR_ENEMY));
}

/************************************************************************
* Función: 
    render_shutdown
* Descripción: 
    Encargada de detener el motor de renderizado 
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void render_shutdown(void)
{
    curs_set(1);    //Vuelve a mostrar el cursor
    nodelay(stdscr, FALSE); //Vuelve a habilitar que se tenga que esperar un enter tras una entrada para poder continuar con el programa
    endwin();
}

/************************************************************************
* Función: 
    render_clear_screen
* Descripción: 
    Encargada de limpiar la consola
* Entradas: 
    Ninguna
* Salidas: 
    Ninguna
*************************************************************************/
void render_clear_screen(void)
{
    erase();
}

/************************************************************************
* Función: 
    render_draw
* Descripción: 
    Encargada de renderizar todo el estado del juego en el frame actual
* Entradas: 
    Puntero a instancia de objeto tipo GAME (juego)
* Salidas: 
    Ninguna
*************************************************************************/
void render_draw(const GameState *game)
{
    /*Matriz de caracteres con las siguientes dimensiones
    Altura = Altura área de juego (Definido en config.c)
    Ancho = Ancho área de juego (Definido en config.c)*/
    char board[GAME_HEIGHT][GAME_WIDTH];    
    fill_board(board);  //Deja en blaco el área de juego
    
    //Si la pantalla actual del juego es la de menú principal 
    if (game->screen == GAME_SCREEN_MENU) {
        render_menu();  //Pasa a renderizarla 
        return; //Sale de la función
    }
    
    //Si la pantalla actual del juego es la de menú de ayuda
    if (game->screen == GAME_SCREEN_HELP) {
        render_help();  //Pasa a renderizarla
        return; //Sale de la función
    }
    
    //Si la pantalla actual del juego es la de menú de ingreso de nombre por parte del jugador
    if (game->screen == GAME_SCREEN_NAME_ENTRY) {
        render_name_entry(game);    //Pasa a renderizarla
        return; //Sale de la función
    }
    
    //Si no ha salido de la función en este punto, significa que hay una partida en curso

    //Renderiza el fondo estrellado en pantalla
    put_starfield_on_board(board, game->frame);

    //Mientras i sea menor a la cantidad máxima de proyectiles activos del jugador
#if defined(RECCA_USE_ARM_GAS)
    put_projectiles_on_board(board, game->player_shots, MAX_PLAYER_SHOTS, '|');
#else
    for (int i = 0; i < MAX_PLAYER_SHOTS; ++i) {
        //Si el proyectil guardado en el índice actual se encuentra activo
        if (game->player_shots[i].active) { 
            //Coloca en consola el caracter correspondiente a un proyectil del jugador en las coordenadas de este
            put_char(board, game->player_shots[i].position.x, game->player_shots[i].position.y, '|');   
        }
    }

    //Mientras i sea menor a la cantidad máxima de proyectiles activos de los enemigos
#endif

#if defined(RECCA_USE_ARM_GAS)
    put_projectiles_on_board(board, game->enemy_shots, MAX_ENEMY_SHOTS, 'o');
#else
    for (int i = 0; i < MAX_ENEMY_SHOTS; ++i) {
        //Si el proyectil guardado en el índice actual se encuentra activo
        if (game->enemy_shots[i].active) {
            //Coloca en consola el caracter correspondiente a un proyectil del enemigo en las coordenadas de este
            put_char(board, game->enemy_shots[i].position.x, game->enemy_shots[i].position.y, 'o');
        }
    }

    //Mientras i sea menor a la cantidad máxima de power-ups activos
#endif

    for (int i = 0; i < MAX_POWERUPS; ++i) {
        //Si el power-up guardado en el índice actual se encuentra activo
        if (game->powerups[i].active) {
            //Coloca en consola el caracter correspondiente a un power-up, según su tipo, en las coordenadas de este
            put_char(board,
                     game->powerups[i].position.x,
                     game->powerups[i].position.y,
                     powerup_char_for_type(&game->powerups[i]));
        }
    }

    //Mientras i sea menor a la cantidad máxima de enemigos activos
    for (int i = 0; i < MAX_ENEMIES; ++i) {
        //Si el enemigo guardado en el índice actual se encuentra activo
        if (game->enemies[i].active) {
            //Llama a función para colocar al enemigo en el área de juego
            put_enemy_on_board(board, &game->enemies[i]);
        }
    }

    //Mientras i sea menor a la cantidad máxima de efectos activos
#if defined(RECCA_USE_ARM_GAS)
    put_effects_on_board(board, game->effects, MAX_EFFECTS, '*');
#else
    for (int i = 0; i < MAX_EFFECTS; ++i) {
        //Si el efecto guardado en el índice actual se encuentra activo
        if (game->effects[i].active) {
            //Coloca en consola el caracter correspondiente a un efecto visual en las coordenadas de este
            put_char(board, game->effects[i].position.x, game->effects[i].position.y, '*');
        }
    }

#endif

    render_clear_screen();  //Limpia el área de consola en la que va a ir el texto que indica los estados de la partida

    attron(COLOR_PAIR(COLOR_PAIR_TEXT));    //Establece color por defecto (Blanco)      
    mvprintw(0, 0, "Summer Carnival '92: Recca - texto");   //Imprime el nombre del juego en pantalla
    int charge_percent = (game->player.charge_frames * 100) / PLAYER_CHARGE_MAX;    //Obtiene el procentaje de carga de la bomba del jugador
    /*Imprime en pantalla los datos del estado del juego en el siguiente formato:
        SC:(Puntaje total hasta el momento) L:(Vidas restantes del jugador)/(Cantidad de vidas máximas que puede tener el jugador) R:(Rango/nivel actual) W:(Nombre de arma actual)(Segundos restantes de arma, en caso de que esta sea especial)
            D:(Cantidad de drones del jugador)(Segundos restantes de drones, en caso de tener) CHG:(Porcentaje de carga de la bomba del jugador) NEXT:(Puntaje necesario para llegar al siguiente jefe)*/
    mvprintw(1, 0, "SC:%06d  L:%d/%d  R:%d  W:%s %02ds  D:%d %02ds  CHG:%3d%%  %s  NEXT:%d",
             game->player.score,
             game->player.lives,
             PLAYER_MAX_LIVES,
             game->level,
             weapon_name_for_type(game->player.weapon),
             timer_seconds(game->player.weapon_timer),
             game->player.drone_count,
             timer_seconds(game->player.drone_timer),
             charge_percent,
             game->phase == LEVEL_PHASE_BOSS ? "BOSS" : "NORMAL",
             game->next_boss_score);
    attroff(COLOR_PAIR(COLOR_PAIR_TEXT));


    // --------------------------------- DIBUJADO DEL ÁREA DE JUEGO ---------------------------------


    // --- Borde superior ---

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));  //Establece color COLOR_PAIR_BORDER (Azul)

    mvaddch(2, 0, '+'); //Dibuja "+" en la línea 2, columna cero (esta será la esquina superior izquierda del área de juego)

    //Mientras x sea menor al ancho máximo del área de juego
    for (int x = 0; x < GAME_WIDTH; ++x) {
        //Dibuja "-" en la línea 2, columna x+1 (este será el margen superior del área de juego)
        mvaddch(2, x + 1, '-');
    }
    mvaddch(2, GAME_WIDTH + 1, '+');    //Dibuja "+" en la línea 2, columna con valor del ancho máximo del área de juego + 1 (esta será la esquina superior derecha del área de juego)
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

    // --- Bordes laterales ---

    //Mientras y sea menor a la altura total del área de juego
    for (int y = 0; y < GAME_HEIGHT; ++y) {
        attron(COLOR_PAIR(COLOR_PAIR_BORDER));  //Establece color COLOR_PAIR_BORDER (Azul)
        mvaddch(y + 3, 0, '|');                 //Dibuja caracter '|' en línea y+3, columna 0 (Este será el margen izquierdo)
        mvaddch(y + 3, GAME_WIDTH + 1, '|');    //Dibuja caracter '|' en línea y+3, columna con valor del ancho máximo del área de juego + 1 (Este será el margen derecho)
        attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

        // --- COLOR CORRESPONDIENTE A CADA CARACTER ---
    
        //Mientras x sea menor al ancho total del área de juego
        for (int x = 0; x < GAME_WIDTH; ++x) {
            char value = board[y][x];   //Obtiene el caracter ubicado en esas coordenadas
            int color = color_for_char(value);  //Obtiene el color correspondiente al caracter ubicado en esa casilla
            attron(COLOR_PAIR(color));  //Establece el color correspondiente
            mvaddch(y + 3, x + 1, value);   //Coloca en consola el caracter en las coordenadas equivalentes a su posición en la matriz lógica
            attroff(COLOR_PAIR(color));
        }
    }

    /*Si:
        El contador de invulnerabilidad del jugador es igual a cero
    O:
        El residuo de dividir entre dos el resultado de dividir este contador entre 5, es cero*/
    if (game->player.invulnerable_timer == 0 ||
        (game->player.invulnerable_timer / 5) % 2 == 0) {
        attron(COLOR_PAIR(COLOR_PAIR_PLAYER));  //Establece color COLOR_PAIR_PLAYER (Cyan)
        mvprintw(game->player.position.y + 3, game->player.position.x, "/A\\"); //Dibuja en las coordenadas correspondientes la representación de la nave del jugador
        attroff(COLOR_PAIR(COLOR_PAIR_PLAYER));
    }

    attron(COLOR_PAIR(COLOR_PAIR_PLAYER));  //Establece color COLOR_PAIR_PLAYER (Cyan)
    //Mientras i sea menor a la cantidad de drones con la que cuenta el jugador
    for (int i = 0; i < game->player.drone_count; ++i) {
        //Toma las coordenadas (x,y) del dron del jugador guardado en el índice actual
        Vec2i drone = player_drone_position(&game->player, i);
        mvaddch(drone.y + 3, drone.x + 1, '@'); //Dibuja su represntación en consola en las coordenadas equivalentes a la matriz lógica
    }
    attroff(COLOR_PAIR(COLOR_PAIR_PLAYER));

    /*Si:
        Si la cantidad de frames de carga de la bomba del jugador es mayor a cero
    Y:
        La coordenada y del jugador es mayor a cero
    */
    if (game->player.charge_frames > 0 && game->player.position.y > 0) {
        /*Obtiene el color correspondiente a la carga del jugador de la siguiente forma:
            Si la cantidad de frames de carga es mayor o igual al necesario para activar esta, obtendrá COLOR_PAIR_PLAYER_SHOT, de lo contrario, COLOR_PAIR_PLAYER*/
        int charge_color = game->player.charge_frames >= PLAYER_CHARGE_RELEASE_MIN ?
                           COLOR_PAIR_PLAYER_SHOT :
                           COLOR_PAIR_PLAYER;
        attron(COLOR_PAIR(charge_color));   //Establece el color obtenido
        mvaddch(game->player.position.y + 2, game->player.position.x + 1, 'O'); //Dibuja la representación de la bomba, directamente arriba del jugador en las coordenadas correspondientes
        attroff(COLOR_PAIR(charge_color));
    }

    // --- Borde inferior ---

    attron(COLOR_PAIR(COLOR_PAIR_BORDER));  //Establece color COLOR_PAIR_BORDER (Azul)
    
    //Dibuja "+" en la línea con valor de la altura total del juego + 3, columna cero (esta será la esquina inferior izquierda del área de juego)
    mvaddch(GAME_HEIGHT + 3, 0, '+');

    //Mientras x sea menor al ancho máximo del área de juego
    for (int x = 0; x < GAME_WIDTH; ++x) {
        //Dibuja "-" en la línea con valor de la altura total del juego + 3, columna x+1 (este será el margen inferior del área de juego)
        mvaddch(GAME_HEIGHT + 3, x + 1, '-');
    }
    mvaddch(GAME_HEIGHT + 3, GAME_WIDTH + 1, '+');  //Dibuja "+" en la línea con valor de la altura total del juego + 3, columna con valor del ancho máximo del área de juego + 1 (esta será la esquina inferior derecha del área de juego)
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER));

    //Si el temporizador del mensaje de estatus es mayor a cero, significa que hay un mensaje que debe ser desplegado, por tanto:
    if (game->status_message_timer > 0) {
        attron(COLOR_PAIR(COLOR_PAIR_PLAYER_SHOT)); //Establece color COLOR_PAIR_PLAYER_SHOT (Azul)
        render_centered(GAME_HEIGHT + 4, game->status_message); //Dibuja el mensaje en pantalla justo debajo del área de juego
        attroff(COLOR_PAIR(COLOR_PAIR_PLAYER_SHOT));
    }

    //Si la pantalla del juego ha cambiado a GAME OVER
    if (game->screen == GAME_SCREEN_GAME_OVER) {
        render_game_over(game); //Pasa a renderizarla

    //Si la pantalla del juego ha cambiado al menú de pausa    
    } else if (game->screen == GAME_SCREEN_PAUSED) {
        render_pause_overlay(); //Pasa a renderizarla
        
        //Si la pantalla del juego ha cambiado a la de un nivel de jefe    
    } else if (game->phase == LEVEL_PHASE_BOSS) {
        render_boss_health_bar(game);   //Procede a renderizar la barra de vida del jefe
    }

    refresh();  //Hace un refresco de pantalla
}
