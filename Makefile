CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Iinclude
LDLIBS = -lncursesw
TARGET := recca_text
SRC := src/main.c src/game.c src/input.c src/render.c src/player.c src/enemy.c src/projectile.c src/collision.c src/effect.c src/powerup.c src/highscore.c
HDR := include/config.h include/types.h include/game.h include/input.h include/render.h include/player.h include/enemy.h include/projectile.h include/collision.h include/effect.h include/powerup.h include/highscore.h

ifeq ($(OS),Windows_NT)
	TARGET := recca_text.exe
	CFLAGS += -IC:/msys64/ucrt64/include/ncursesw
	LDFLAGS += -LC:/msys64/ucrt64/lib
	RM = del /Q
else
	RM = rm -f
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
