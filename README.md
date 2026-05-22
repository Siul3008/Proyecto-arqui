# Summer Carnival '92: Recca - adaptacion en modo texto

Este proyecto es una adaptacion academica en terminal de un shooter vertical inspirado en
`Summer Carnival '92: Recca`. No usa codigo ni recursos del juego original.

## Alcance inicial

- Nave del jugador.
- Movimiento en cuatro direcciones.
- Disparos del jugador.
- Enemigos que aparecen desde la parte superior.
- Proyectiles enemigos.
- Colisiones.
- Vidas y puntaje.

## Controles

- `W`: mover arriba.
- `A`: mover izquierda.
- `S`: mover abajo.
- `D`: mover derecha.
- `Espacio`: disparar.
- `Q`: salir.

## Compilacion

```sh
make
```

## Ejecucion

```sh
make run
```

En Windows, si no se usa `make`, se puede compilar con GCC:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/game.c src/input.c src/render.c -o recca_text.exe
```

## Diseno

La logica del juego esta separada en modulos:

- `game`: estado, movimiento, enemigos, proyectiles y colisiones.
- `input`: lectura no bloqueante del teclado con `ncurses`.
- `render`: dibujo de la pantalla en terminal con `ncurses`.
- `config`: constantes del juego.

Esta separacion facilita medir funciones criticas y reemplazar partes por rutinas AA32 en etapas posteriores.
