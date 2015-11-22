gcc -std=c11 %1 -Wall -Wextra ^
	src/main.c ^
	src/board.c ^
	src/chunk.c ^
	-lpdcurses ^
	-o gol
