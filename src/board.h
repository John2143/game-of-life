#ifndef board_H
#define board_H

#include <stdlib.h>

#include "chunk.h"

#define DEFAULTWIDTH 8

struct board{
	struct chunk *chunks;
	int size;
	int maxSize;
};

void iterateBoard(struct board *board);
void addChunk(struct board *b, int x, int y);
struct board *createBoard();
void freeBoard(struct board *board);

#endif
