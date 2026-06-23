# Build configuration
CC = gcc
CFLAGS_BASE = -std=gnu11 -Wall -Wextra -Wpedantic -Iinclude
PLATFORM_CFLAGS =
LDFLAGS =
LDLIBS = -lncursesw

TARGET := recca_text_original
TARGET_ORIGINAL := recca_text_original
TARGET_GAS := recca_text_gas

SRC := src/main.c src/game.c src/input.c src/render.c src/player.c src/enemy.c src/projectile.c src/collision.c src/effect.c src/powerup.c src/highscore.c src/sound.c
GAS_SRC := src/render_arm.s src/collision_arm.s src/projectile_arm.s
GAS_TARGETS :=

HDR := include/config.h include/types.h include/game.h include/input.h include/render.h include/player.h include/enemy.h include/projectile.h include/collision.h include/effect.h include/powerup.h include/highscore.h include/sound.h include/arm_gas.h

ARM_GAS_MACHINES := armv6l armv7l armv8l

ifeq ($(OS),Windows_NT)
	TARGET := recca_text_original.exe
	TARGET_ORIGINAL := recca_text_original.exe
	TARGET_GAS := recca_text_gas.exe
	PLATFORM_CFLAGS += -IC:/msys64/ucrt64/include/ncursesw -IC:/msys64/ucrt64/include/SDL2
	LDFLAGS += -LC:/msys64/ucrt64/lib
	LDLIBS += -lSDL2_mixer -lSDL2
	RM = del /Q
else
	PLATFORM_CFLAGS += $(shell pkg-config --cflags sdl2 SDL2_mixer)
	LDLIBS += $(shell pkg-config --libs sdl2 SDL2_mixer)
	RM = rm -f

	UNAME_M := $(shell uname -m)
	ifneq (,$(filter $(ARM_GAS_MACHINES),$(UNAME_M)))
		GAS_TARGETS := $(TARGET_GAS)
	endif

	ifeq ($(USE_ARM_GAS),1)
		GAS_TARGETS := $(TARGET_GAS)
	endif
endif

CFLAGS = $(CFLAGS_BASE) $(PLATFORM_CFLAGS)
CFLAGS_GAS = $(CFLAGS_BASE) $(PLATFORM_CFLAGS) -DRECCA_USE_ARM_GAS

.PHONY: all both original gas clean run run-gas

all: $(TARGET_ORIGINAL) $(GAS_TARGETS)

both: $(TARGET_ORIGINAL) $(TARGET_GAS)

original: $(TARGET_ORIGINAL)

gas: $(TARGET_GAS)

$(TARGET_ORIGINAL): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET_ORIGINAL) $(LDFLAGS) $(LDLIBS)

$(TARGET_GAS): $(SRC) $(GAS_SRC) $(HDR)
	$(CC) $(CFLAGS_GAS) $(SRC) $(GAS_SRC) -o $(TARGET_GAS) $(LDFLAGS) $(LDLIBS)

run: $(TARGET_ORIGINAL)
	./$(TARGET_ORIGINAL)

run-gas: $(TARGET_GAS)
	./$(TARGET_GAS)

clean:
	-$(RM) $(TARGET_ORIGINAL) $(TARGET_GAS)
