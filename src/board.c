#include "board.h"

void iterateBoard(struct board *board){
	for(int i = 0; i < board->size; i++){
		calculateChunk(&board->chunks[i]);
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

int getChunkPos(struct board *b, int x, int y){
	for(int i = 0; i < b->size; i++){
		if(b->chunks[i].locx == x && b->chunks[i].locy == y){
			return i;
		}
	}
	return -1;
}
int moveBoard(struct board *b, int x, int y){
	return setBoard(b, curChunk(b).locx + x, curChunk(b).locy + y);
}
int setBoard(struct board *b, int x, int y){
	int pos = getChunkPos(b, x, y);
	if(pos < 0) return -1;
	b->curChunk = pos;
	return 0;
}
void drawBoard(struct board *b){
	if(b->size < b->curChunk) return;//TODO err
	drawChunk(&curChunk(b));
}
