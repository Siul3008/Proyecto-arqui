# Summer Carnival '92: Recca - adaptación en modo texto

Proyecto academico desarrollado en C como una adaptación en terminal de un
shooter vertical inspirado en `Summer Carnival '92: Recca`.

El objetivo del proyecto es replicar la idea general del juego desde cero,
aplicando programación estructurada, separación por módulos, manejo de memoria
simple, entrada por teclado, renderizado en terminal y medición posterior de
módulos criticos para una posible comparación con rutinas en ensamblador AA32.

No se hace uso de código ni recursos del juego original.

## Estado actual

El juego actualmente incluye:

- Nave del jugador con movimiento en cuatro direcciones.
- Disparo principal.
- Tres tipos de arma: frontal, spread y laser.
- Drones temporales como apoyo de disparo.
- Power-ups temporales para armas y drones.
- Bomba cargada al dejar de disparar.
- Escudo de carga capaz de absorber proyectiles pequeños.
- Enemigos normales con múltiples comportamientos aleatorios.
- Mini jefes y jefes de mayor dificultad.
- Sistema infinito por puntaje y rango.
- Progresión de dificultad de manera gradual.
- Vidas, puntaje y penalización al morir.
- Pantalla de pausa.
- Menú de ayuda.
- Pantalla de Game Over con resumen de partida.
- Tabla local de mejores puntuaciones.
- Ingreso de nombre del jugador al terminar la partida.

## Controles

- `W` o flecha arriba: mover nave jugador hacia arriba.
- `A` o flecha izquierda: mover nave jugador hacia izquierda.
- `S` o flecha abajo: mover nave jugador hacia abajo.
- `D` o flecha derecha: mover nave jugador hacia derecha.
- `Espacio`: disparar.
- Soltar `Espacio`: cargar bomba.
- Volver a presionar `Espacio`: liberar bomba cargada.
- `P`: pausar o continuar partida.
- `H`: abrir o cerrar menú de ayuda.
- `R`: reiniciar partida desde Game Over.
- `Q`: salir.
- `ENTER`: iniciar partida o guardar nombre en ranking.
- `BACKSPACE`: borrar letra al ingresar nombre.

## Ranking

Cuando el jugador pierde todas sus vidas, el juego pide un nombre de máximo
10 caracteres. Esto para guardar el puntaje del jugador en una tabla local que contiene los mejores
resultados hasta el momento.

El nombre del archivo que guarda dichos puntajes es:

```sh
highscores.txt
```

Este archivo se crea al ejecutar el juego por primera vez y guarda:

- Nombre del jugador.
- Puntaje.
- Rango alcanzado.
- Cantidad de jefes derrotados.

## Herramientas utilizadas en el desarrollo

- Lenguaje: C.
- Estándar: C11.
- Librería de terminal: ncurses / ncursesw.
- Compilador utilizado en Windows: GCC (MSYS2 UCRT64).
- Compilador utilizado en Linux: GCC.
- Sistema de compilación: Makefile.
- Editor utilizado: Visual Studio Code.

## Compilación en Windows

El proyecto está preparado para compilar con MSYS2 UCRT64 y `mingw32-make`.

Desde PowerShell, dentro de la carpeta del proyecto:

```sh
mingw32-make
```

Esto genera:

```sh
recca_text_original.exe
```

Para ejecutar:

```sh
./recca_text_original.exe
```

Si se hace uso de la terminal UCRT64 de MSYS2, también es posible ejecutar:

```sh
mingw32-make run
```

## Compilación en Raspberry Pi

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
./recca_text_original
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

## Diseño general

La lógica se encuentra dividida en módulos pequeños para facilitar mantenimiento y
pruebas. La idea es que cada archivo tenga una responsabilidad definida:

- `game` coordina el estado general.
- `player` maneja la nave y sus armas.
- `enemy` maneja enemigos, jefes y patrones.
- `projectile` mueve disparos del jugador y enemigos.
- `collision` calcula impactos y consecuencias de los mismos.
- `powerup` controla mejoras temporales.
- `highscore` guarda y ordena mejores puntuaciones.
- `render` solo dibuja en pantalla lo que ya existe en el estado.
- `input` traduce entradas del usuario a acciones del juego.

Esta separación es de utilidad para así mantener un codigo mas limpio y tambien agilizar
la elección de funciones específicas para medir su rendimiento y/u optimizar las mismas en caso se ser necesario.

## Instalación de dependencias

### Windows usando MSYS2 UCRT64

Abrir la terminal "MSYS2 UCRT64" y ejecutar:

pacman -Syu

Si MSYS2 pide cerrar la terminal, cerrarla, abrir nuevamente "MSYS2 UCRT64" y ejecutar:

pacman -Su

Luego instalar las dependencias del proyecto:

pacman -S --needed base-devel git
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain
pacman -S --needed mingw-w64-ucrt-x86_64-make
pacman -S --needed mingw-w64-ucrt-x86_64-ncurses
pacman -S --needed mingw-w64-ucrt-x86_64-SDL2
pacman -S --needed mingw-w64-ucrt-x86_64-SDL2_mixer
pacman -S --needed mingw-w64-ucrt-x86_64-mpg123
pacman -S --needed mingw-w64-ucrt-x86_64-pkgconf

Verificar instalación:

gcc --version
g++ --version
mingw32-make --version

Compilar y ejecutar:

cd /f/U/Arqui/ProyectoFinal/SummerCarnival92Recca
mingw32-make
./recca_text_original.exe


### Linux / Ubuntu / Debian

Actualizar repositorios:

sudo apt update

Instalar dependencias:

sudo apt install -y build-essential git make gcc g++
sudo apt install -y libncurses-dev libsdl2-dev libsdl2-mixer-dev
sudo apt install -y mpg123 pkg-config

Compilar y ejecutar:

make
./recca_text_original


### Raspberry Pi 4

En Raspberry Pi OS, actualizar repositorios:

sudo apt update

Instalar dependencias:

sudo apt install -y build-essential git make gcc g++
sudo apt install -y libncurses-dev libsdl2-dev libsdl2-mixer-dev
sudo apt install -y mpg123 pkg-config

Compilar y ejecutar:

make
./recca_text_original

En Raspberry Pi OS de 32 bits, el Makefile detecta automaticamente arquitecturas ARM AArch32 como armv6l, armv7l o armv8l y agrega los archivos de ensamblador GAS:

src/render_arm.s
src/collision_arm.s

Tambien se puede forzar esa ruta de compilacion con:

make USE_ARM_GAS=1

Para generar ambos binarios de medicion:

make both

Esto genera:

./recca_text_original
./recca_text_gas

El primero usa solo la ruta C. El segundo activa RECCA_USE_ARM_GAS y compila los archivos .s. Para comparar tiempos, ejecutar ambos por separado y revisar las mediciones mostradas por el sistema de timers.

Optimizaciones agregadas para la rama ORIGINAL-Performance-Metrics:

- render.c: el llenado del tablero logico, la seleccion de color por caracter y el copiado de proyectiles/efectos al tablero pueden ejecutarse en GAS para ARM.
- collision.c: las verificaciones frecuentes de posiciones, rangos, proyectil contra jugador, proyectil contra enemigo, jugador contra enemigo y suma saturada de timers pueden ejecutarse en GAS para ARM.
- En plataformas que no son ARM 32-bit, el juego mantiene las funciones C originales como fallback.


### Nota sobre audio

El juego usa SDL2_mixer para reproducir archivos .wav.

Actualmente los audios se encuentran en:

assets/music/
assets/sfx/

El paquete mpg123 se incluye por compatibilidad futura en caso de usar archivos .mp3.
