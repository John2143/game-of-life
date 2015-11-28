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

//Return a pointer to the index-th chunk
#define getChunk(b, index) (&(b)->chunks[index])
//#define getChunk(b, index) ((b)->chunk + (index) * sizeof(struct chunk))

//The current chunk
#define curChunk(b) getChunk(b, b->curChunk)
//Get the next uninitialized memeory block. MAY BE OUT OF THE RANGE OF b->chunks
#define nextChunk(b) getChunk(b, b->size)

//Mutate the board by one time unit.
//TODO: Make memory more efficient by calculating a better order
//TODO: Decide how often to run the GC
void iterateBoard(struct board *board);
//This will generate new chunks if they will be needed in the next iteration
//Returns the number of chunks generated.
int generateNewChunks(struct board *b);
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

//Remove all chunks that are 0, possibly compressing the buffer
//Returns the number of chunks removed
//This method may be referenced as "The GC"
int collectGarbage(struct board *b);

#endif
