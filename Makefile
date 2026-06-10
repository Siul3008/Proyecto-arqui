#Declaración de variables para compilación
CC = gcc	#Compilador a usar
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude	#Banderas para el compilador (Uso estandar c11)(Habilitar todas las advertencias posibles)(Incluir archivos de encabezado .h)
LDLIBS = -lncursesw	#Librerias de linker necesarias, en este caso, ncurses
TARGET := recca_text	#Nombre del archivo destino
SRC := src/main.c src/game.c src/input.c src/render.c src/player.c src/enemy.c src/projectile.c src/collision.c src/effect.c src/powerup.c src/highscore.c src/sound.c	#Lista de archivos .c que deben ser compilados
HDR := include/config.h include/types.h include/game.h include/input.h include/render.h include/player.h include/enemy.h include/projectile.h include/collision.h include/effect.h include/powerup.h include/highscore.h include/sound.h	#Lista de archivos .h de los que dependen los .c
 
#Si el sistema operativo es windows, compilar con los siguientes parámetros
ifeq ($(OS),Windows_NT)
	TARGET := recca_text.exe
	CFLAGS += -IC:/msys64/ucrt64/include/ncursesw -IC:/msys64/ucrt64/include/SDL2
	LDFLAGS += -LC:/msys64/ucrt64/lib
	LDLIBS += -lSDL2_mixer -lSDL2
	RM = del /Q
else	#De lo contrario, con estos otros
	LDLIBS += -lSDL2_mixer -lSDL2
	RM = rm -f
endif

.PHONY: all clean run	#Declara que los parámetros a continuación no son archivos como tal, solo nombres de variables/comandos

all: $(TARGET)
#Declara archivo resultante y las dependencias que este tendrá
$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LDLIBS) 

run: $(TARGET)	#Ejecutar programa que acaba de ser compilado
	./$(TARGET)

clean:	#Limpiar archivos residuales
	$(RM) $(TARGET)
