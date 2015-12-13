CC=gcc -std=c11
CFLAGS=-Wall -Wextra -pedantic -Iinclude
LDFLAGS=
FILES=main.c board.c chunk.c global.c input.c io.c
SOURCES=$(addprefix src/, $(FILES))
EXECUTABLE=gol
LIBRARIES=-lpdcurses

all:
	$(CC) $(CFLAGS) -g $(SOURCES) $(LIBRARIES) -o $(EXECUTABLE)

fast:
	$(CC) $(CFLAGS) -Ofast $(SOURCES) $(LIBRARIES) -o $(EXECUTABLE)

clean:
	rm *o
