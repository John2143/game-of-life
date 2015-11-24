gcc -std=c11 %1 -Wall -Wextra -pedantic ^
	src/main.c ^
	src/board.c ^
	src/chunk.c ^
	-lpdcurses ^
	-o gol
