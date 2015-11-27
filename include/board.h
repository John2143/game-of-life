#ifndef board_H
#define board_H

#include <stdlib.h>
#include <curses.h>

#include "chunk.h"

#define DEFAULTWIDTH 8

struct board{
	struct chunk *chunks;
	int size;
	int maxSize;
	int curChunk;
	int iterations;
};

//Mutate the board by one time unit.
//TODO: Make memory more efficient by calculating a better order
//TODO: Collect garbage(0) chunks
void iterateBoard(struct board *board);
//Adds a new chunk to the board, possibly doubling the stack size if it needs room
void addChunk(struct board *b, int x, int y);
//Creates a new empty board. Must call freeBoard() to release the pointer.
struct board *createBoard();
void freeBoard(struct board *board);


//Get the chunk at the specified point. -1 on empty. O(n)
int getChunkAtPos(struct board *b, int x, int y);
//Move the board by x, y units right/down. Negative for left/up
int moveBoard(struct board *b, int x, int y);
//Move the board to the position of x, y. O(getChunkAtPos())
int setBoard(struct board *b, int x, int y);
//Draws the current chunk as selected by the board. Exits if there is no chunk selected
//chunk.h: drawChunk does most of the heavy lifting, with drawboard just displaying info
//TODO: return or handle error
void drawBoard(struct board *b);

#define curChunk(b) ((b)->chunks[(b)->curChunk])

#endif
