#include "board.h"

void iterateBoard(struct board *board){
	for(int i = 0; i < board->size; i++){
		iterateChunk(&board->chunks[i]);
	}
}

void addChunk(struct board *b, int x, int y){
	if(b->size >= b->maxSize){
		b->maxSize *= 2;
		b->chunks = realloc(b->chunks, b->maxSize * sizeof(struct chunk));
	}
	struct chunk *n = &b->chunks[b->size];
	memset(&n->board, 0, CHUNKSIZE2);
	n->locx = x;
	n->locy = y;
	b->size++;
}

struct board *createBoard(){
	struct board *b = malloc(sizeof *b);
	b->size = 0;
	b->maxSize = DEFAULTWIDTH;
	b->chunks = malloc(DEFAULTWIDTH * sizeof(struct chunk));
	return b;
}

void freeBoard(struct board *board){
	free(board->chunks);
	free(board);
}
