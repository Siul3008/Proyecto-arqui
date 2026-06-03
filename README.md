# Summer Carnival '92: Recca - adaptacion en modo texto

Proyecto academico desarrollado en C como una adaptacion en terminal de un
shooter vertical inspirado en `Summer Carnival '92: Recca`.

El objetivo del proyecto es replicar la idea general del juego desde cero,
aplicando programacion estructurada, separacion por modulos, manejo de memoria
simple, entrada por teclado, renderizado en terminal y medicion posterior de
partes criticas para una posible comparacion con rutinas en ensamblador AA32.

No se usa codigo ni recursos del juego original.

## Estado actual

El juego ya incluye:

- Nave del jugador con movimiento en cuatro direcciones.
- Disparo principal.
- Tres tipos de arma: frontal, spread y laser.
- Drones temporales como apoyo de disparo.
- Power-ups temporales para armas y drones.
- Bomba cargada al dejar de disparar.
- Escudo de carga capaz de absorber proyectiles pequenos.
- Enemigos normales con varios comportamientos.
- Mini jefes y jefes de mayor dificultad.
- Sistema infinito por puntaje y rango.
- Progresion de dificultad mas gradual.
- Vidas, puntaje y penalizacion al morir.
- Pausa.
- Pantalla de ayuda.
- Pantalla de Game Over con resumen de partida.
- Tabla local de mejores puntuaciones.
- Ingreso de nombre al terminar la partida.

## Controles

- `W` o flecha arriba: mover arriba.
- `A` o flecha izquierda: mover izquierda.
- `S` o flecha abajo: mover abajo.
- `D` o flecha derecha: mover derecha.
- `Espacio`: disparar.
- Soltar `Espacio`: cargar bomba.
- Volver a presionar `Espacio`: liberar bomba cargada.
- `P`: pausar o continuar.
- `H`: abrir o cerrar ayuda.
- `R`: reiniciar desde Game Over.
- `Q`: salir.
- `ENTER`: iniciar partida o guardar nombre en ranking.
- `BACKSPACE`: borrar letra al ingresar nombre.

## Ranking

Cuando el jugador pierde todas sus vidas, el juego pide un nombre de maximo
10 caracteres. Luego guarda la puntuacion en una tabla local de mejores
resultados.

El archivo generado se llama:

```sh
highscores.txt
```

Este archivo se crea al ejecutar el juego y guarda:

- Nombre del jugador.
- Puntaje.
- Rango alcanzado.
- Cantidad de jefes derrotados.

## Herramientas

- Lenguaje: C.
- Estandar: C11.
- Libreria de terminal: ncurses / ncursesw.
- Compilador recomendado en Windows: GCC de MSYS2 UCRT64.
- Sistema de compilacion: Makefile.
- Editor recomendado: Visual Studio Code.

## Compilacion en Windows

El proyecto esta preparado para compilar con MSYS2 UCRT64 y `mingw32-make`.

Desde PowerShell, dentro de la carpeta del proyecto:

```sh
mingw32-make
```

Esto genera:

```sh
recca_text.exe
```

Para ejecutar:

```sh
./recca_text.exe
```

Si se usa la terminal UCRT64 de MSYS2, tambien se puede ejecutar:

```sh
mingw32-make run
```

## Compilacion en Raspberry Pi

En Raspberry Pi se recomienda instalar GCC, make y ncurses:

```sh
sudo apt update
sudo apt install build-essential libncurses-dev
```

Luego, dentro de la carpeta del proyecto:

```sh
make
```

Para ejecutar:

```sh
./recca_text
```

## Limpieza

Para borrar el ejecutable generado:

```sh
mingw32-make clean
```

En Raspberry Pi o Linux:

```sh
make clean
```

## Estructura del proyecto

```text
include/
  collision.h    Declaraciones de colisiones.
  config.h       Constantes generales del juego.
  effect.h       Efectos visuales simples.
  enemy.h        Enemigos, jefes y limites de hitbox.
  game.h         Estado general del juego.
  highscore.h    Tabla de mejores puntuaciones.
  input.h        Entrada por teclado.
  player.h       Jugador, armas, drones y bomba cargada.
  powerup.h      Power-ups.
  projectile.h   Proyectiles.
  render.h       Renderizado en terminal.
  types.h        Tipos compartidos.

src/
  collision.c    Logica de colisiones.
  effect.c       Actualizacion de efectos.
  enemy.c        Movimiento, disparo y aparicion de enemigos.
  game.c         Flujo principal del juego y estados.
  highscore.c    Carga, guardado y ordenamiento del ranking.
  input.c        Lectura no bloqueante del teclado.
  main.c         Inicializacion y ciclo principal.
  player.c       Logica del jugador.
  powerup.c      Movimiento y aparicion de power-ups.
  projectile.c   Movimiento de proyectiles.
  render.c       Dibujo de pantalla, HUD y menus.
```

## Diseno general

La logica esta dividida en modulos pequenos para facilitar mantenimiento y
pruebas. La idea es que cada archivo tenga una responsabilidad clara:

- `game` coordina el estado general.
- `player` maneja la nave y sus armas.
- `enemy` maneja enemigos, jefes y patrones.
- `projectile` mueve disparos del jugador y enemigos.
- `collision` decide impactos y consecuencias.
- `powerup` controla mejoras temporales.
- `highscore` guarda y ordena mejores puntuaciones.
- `render` solo dibuja lo que ya existe en el estado.
- `input` traduce teclas a acciones del juego.

Esta separacion ayuda a mantener un codigo mas limpio y tambien facilita
escoger funciones especificas para medir rendimiento u optimizar despues.
