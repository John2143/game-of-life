#ifndef board_H
#define board_H

#include <stdlib.h>

#include "chunk.h"

#define DEFAULTWIDTH 8

struct board{
	struct chunk *chunks;
	int size;
	int maxSize;
	int curChunk;
};

void iterateBoard(struct board *board);
void addChunk(struct board *b, int x, int y);
struct board *createBoard();
void freeBoard(struct board *board);

int getChunkAtPos(struct board *b, int x, int y);
int moveBoard(struct board *b, int x, int y);
int setBoard(struct board *b, int x, int y);
void drawBoard(struct board *b);

#define curChunk(b) (b->chunks[b->curChunk])

#endif
