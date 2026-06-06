#ifndef RECCA_CONFIG_H
#define RECCA_CONFIG_H

#define GAME_WIDTH 60 //Ancho del área jugable en caracteres.
#define GAME_HEIGHT 24 //Altura del área jugable en caracteres.
#define STARFIELD_DOT_COUNT 28 //Número de puntos en el campo de estrellas. Se recomienda un número par para evitar parpadeos.
#define STARFIELD_SCROLL_INTERVAL 2 //Intervalo en frames para desplazar el campo de estrellas. Un valor más bajo hará que las estrellas se muevan más rápido.

#define MAX_PLAYER_SHOTS 48 //Número máximo de disparos del jugador en pantalla al mismo tiempo. Se recomienda un número par para evitar parpadeos.
#define MAX_ENEMIES 16 //Número máximo de enemigos en pantalla al mismo tiempo. Se recomienda un número par para evitar parpadeos.
#define MAX_ENEMY_SHOTS 40 //Número máximo de disparos de enemigos en pantalla al mismo tiempo. Se recomienda un número par para evitar parpadeos.
#define MAX_POWERUPS 6 //Número máximo de power-ups en pantalla al mismo tiempo. Se recomienda un número par para evitar parpadeos.
#define MAX_HIGH_SCORES 5 //Número máximo de puntuaciones altas a almacenar.
#define PLAYER_NAME_MAX_LENGTH 10 //Longitud máxima del nombre del jugador.
#define HIGHSCORE_FILE_NAME "highscores.txt" //Nombre del archivo para almacenar las puntuaciones altas.

#define MAX_EFFECTS 16 //Número máximo de efectos en pantalla al mismo tiempo.
#define EXPLOSION_DURATION 5 //Duración en frames de una explosión. Un valor más bajo hará que las explosiones desaparezcan más rápido.
#define STATUS_MESSAGE_LENGTH 40 //Longitud máxima del mensaje de estado.
#define STATUS_MESSAGE_DURATION 70 //Duración en frames del mensaje de estado.

#define WAVE_BASE_ENEMY_COUNT 8 //Número base de enemigos por oleada.
#define WAVE_SPAWN_INTERVAL 18 //Intervalo en frames entre cada aparición de enemigo dentro de una oleada. Un valor más bajo hará que los enemigos aparezcan más rápido.
#define WAVE_BREAK_FRAMES 80 //Número de frames de descanso entre oleadas. Un valor más bajo hará que las oleadas se sucedan más rápidamente.

#define SCORE_RANK_INTERVAL 2500 //Intervalo de puntuación para subir de rango. Un valor más bajo hará que los rangos se suban más rápido.
#define BOSS_SCORE_INTERVAL 1400 //Intervalo de puntuación para que aparezca un jefe. Un valor más bajo hará que los jefes aparezcan más rápido.
#define BOSS_SCORE_INTERVAL_GROWTH 400 //Cantidad de aumento en el intervalo de puntuación para cada jefe adicional. Un valor más bajo hará que los jefes aparezcan más rápido a medida que aumente la puntuación.
#define MINI_BOSS_HEALTH 8 //Salud del jefe mini.
#define STAGE_BOSS_HEALTH 16 //Salud del jefe de etapa.
#define MINI_BOSS_SCORE 500 //Puntuación por derrotar un jefe mini.
#define STAGE_BOSS_SCORE 1000 //Puntuación por derrotar un jefe de etapa.


#define PLAYER_MAX_LIVES 3 //Número máximo de vidas del jugador.
#define PLAYER_START_LIVES PLAYER_MAX_LIVES //Número de vidas del jugador al comenzar.
#define EXTRA_LIFE_SCORE_INTERVAL 1000 //Intervalo de puntuación para obtener una vida extra.
#define PLAYER_START_X (GAME_WIDTH / 2) //Posición inicial del jugador en el eje X.
#define PLAYER_START_Y (GAME_HEIGHT - 2) //Posición inicial del jugador en el eje Y.
#define PLAYER_INVULNERABLE_FRAMES 45 //Número de frames de invulnerabilidad después de recibir un daño.
#define MAX_PLAYER_DRONES 2 //Número máximo de drones del jugador en pantalla al mismo tiempo.
#define PLAYER_DRONE_OFFSET_X 4 //Desplazamiento en el eje X para los drones del jugador.
#define PLAYER_DRONE_OFFSET_Y 1 //Desplazamiento en el eje Y para los drones del jugador.
#define PLAYER_WEAPON_DURATION_FRAMES 515 //Duración en frames del arma del jugador.
#define PLAYER_WEAPON_SHORT_DURATION_FRAMES 390 //Duración en frames del arma corta del jugador. Un valor más bajo hará que el arma corta desaparezca más rápido.
#define PLAYER_WEAPON_MEDIUM_DURATION_FRAMES 460 //Duración en frames del arma media del jugador. Un valor más bajo hará que el arma media desaparezca más rápido.
#define PLAYER_WEAPON_LONG_DURATION_FRAMES 560 //Duración en frames del arma larga del jugador. Un valor más bajo hará que el arma larga desaparezca más rápido.
#define PLAYER_DRONE_DURATION_FRAMES 1027 //Duración en frames de los drones del jugador.
#define PLAYER_CHARGE_MAX 60 //Carga máxima del jugador.
#define PLAYER_CHARGE_RELEASE_MIN 25 //Carga mínima para liberar un ataque.
#define PLAYER_CHARGE_SHIELD_MIN 15 //Carga mínima para activar el escudo.
#define CHARGE_SHIELD_RADIUS 1 //Radio del escudo de carga.
#define CHARGE_BOMB_RADIUS 6 //Radio de la bomba de carga.
#define CHARGE_BOMB_DAMAGE 3 //Daño de la bomba de carga.

#define POWERUP_DROP_DIVISOR 4 //Divisor para calcular la probabilidad de que un enemigo suelte un power-up al ser derrotado. Un valor más bajo hará que los power-ups sean más comunes.
#define POWERUP_MOVE_INTERVAL 3 //Intervalo en frames para mover los power-ups. Un valor más bajo hará que los power-ups se muevan más rápido.

#define PLAYER_SHOT_COOLDOWN 2 //Número de frames entre cada disparo del jugador. Un valor más bajo hará que el jugador pueda disparar más rápido.
#define PLAYER_SHOT_MOVE_INTERVAL 1 //Intervalo en frames para mover los disparos del jugador. Un valor más bajo hará que los disparos se muevan más rápido.

#define ENEMY_BASE_MOVE_INTERVAL 4 //Intervalo en frames para mover los enemigos. Un valor más bajo hará que los enemigos se muevan más rápido.
#define ENEMY_SHOT_BASE_MOVE_INTERVAL 2 //Intervalo en frames para mover los disparos de los enemigos. Un valor más bajo hará que los disparos de los enemigos se muevan más rápido. 

#define FRAME_DELAY_MS 40 //Número de milisegundos a esperar entre cada frame del juego. Un valor más bajo hará que el juego se ejecute más rápido, pero puede causar problemas de rendimiento en sistemas más lentos.

#endif
