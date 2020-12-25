.PHONY: all external tests

ifeq ($(OS),Windows_NT)
	PLATFORM_OS = WINDOWS
else
	UNAMEOS = $(shell uname)
	ifeq ($(UNAMEOS),Linux)
		PLATFORM_OS = LINUX
	endif
	ifeq ($(UNAMEOS),FreeBSD)
		PLATFORM_OS = BSD
	endif
	ifeq ($(UNAMEOS),OpenBSD)
		PLATFORM_OS = BSD
	endif
	ifeq ($(UNAMEOS),NetBSD)
		PLATFORM_OS = BSD
	endif
	ifeq ($(UNAMEOS),DragonFly)
		PLATFORM_OS = BSD
	endif
	ifeq ($(UNAMEOS),Darwin)
		PLATFORM_OS = OSX
	endif
endif
CC = gcc
CFILES = src/main.c src/common.c
INCLUDE_PATHS = -Isrc -Iexternal/raylib/src
LDFLAGS = -Lexternal/lib
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: external $(CFILES)
	$(CC) $(CFILES) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -o build/hero_forester 

external:
	cd external/raylib/src && $(MAKE) PLATFORM=PLATFORM_DESKTOP

external_clean:
	cd external/raylib/src && $(MAKE) clean

tests:
	$(CC) src/tests.c src/common.c -lm -o build/tests
	./build/tests
