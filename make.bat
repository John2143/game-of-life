gcc -std=c11 %1 -Wall -Wextra -pedantic ^
	-Iinclude ^
	src/main.c ^
	src/board.c ^
	src/chunk.c ^
	src/input.c ^
	src/io.c ^
	-lpdcurses ^
	-o gol
