#include "board.h"

//TODO incomplete
int generateNewChunks(struct board *b){
	int size = b->size;
	for(int i = 0; i < size; i++){
		struct chunk c = b->chunks[i];
		int x = c.locx, y = c.locy;
		(void) x;
		(void) y;
	}
	return b->size - size;
}

void iterateBoard(struct board *board){
	bval (*change)[CHUNKSIZE2] = malloc(board->size * CHUNKSIZE2 * sizeof(bval));
	for(int i = 0; i < board->size; i++){
		calculateChunk(&board->chunks[i], change[i]);
	}
	for(int i = 0; i < board->size; i++){
		applyChange(&board->chunks[i], change[i]);
	}
	free(change);
	board->iterations++;
}

int collectGarbage(struct board *b){
	int marked[b->size];
	int markednum = 0;
	for(int i = 0; i < b->size; i++){
		marked[i] = 1; //empty
		for(int j = 0; j < CHUNKSIZE2; j++){
			if(b->chunks[i].board[j]){
				marked[i] = 0;
				break;
			}
		}
		if(marked[i]) markednum++;
	}

	//This is just the shell for the algorithm here
	//TODO optimize this a lot
	int first = 0;
	int last = b->size;
	while(1){
		while(first < last && !marked[first]) first++;
		while(last > first && marked[last]) last--;
		if(first >= last) break;
		b->size = last;
		memcpy(&b->chunks[first], &b->chunks[last], sizeof(struct chunk));
		for(int i = 0; i < 8; i++){
			if(b->chunks[first].neighbors[i]){
				//make all neighbors have the new pointer
				b->chunks[first].neighbors[i]->neighbors[neighborOpposite(i)] = &b->chunks[first];
			}
		}
		first++;
		last--;
	}
	return markednum;
}

static int checkNeighborhood(struct chunk *a, struct chunk *b, int x, int y, int j, int k){
	//x = b->locx
	//y = b->locy
	if ((x + k == a->locx) && (y + j == a->locy)){
		b->neighbors[neighborDelta(j, k)] = a;
		a->neighbors[rneighborDelta(j, k)] = b;
		return 1;
	}
	return 0;
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
	int i;
	for(i = 0; i < 8; i++){
		n->neighbors[i] = NULL;
	}
	for(i = 0; i < b->size; i++){
		#define X(j, k) \
		if(checkNeighborhood(&b->chunks[i], n, x, y, j, k)) continue;
		EXPAND_DIRS
		#undef X
	}
	b->size++;
}

struct board *createBoard(){
	struct board *b = malloc(sizeof *b);
	b->size = 0;
	b->maxSize = DEFAULTWIDTH;
	b->chunks = malloc(DEFAULTWIDTH * sizeof(struct chunk));
	b->iterations = 0;
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
	mvprintw(CHUNKSIZE + 1, 0, "At (%i, %i), Iteration (%i)",
		curChunk(b).locx, curChunk(b).locy,
		b->iterations
	);
}
