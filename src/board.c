#include "board.h"

int generateNewChunks(struct board *b){
	int size = b->size;
	for(int i = 0; i < size; i++){
		struct chunk *c = getChunk(b, i);
		int x = c->locx, y = c->locy;
		int k = newChunks(c);
		if(k & 0x1) addChunk(b, x + 1, y, NULL);//Right
		if(k & 0x2) addChunk(b, x - 1, y, NULL);//Left
		if(k & 0x4) addChunk(b, x, y + 1, NULL);//Down
		if(k & 0x8) addChunk(b, x, y - 1, NULL);//Up
	}
	return b->size - size;
}

void iterateBoard(struct board *board){
	if(!board->untilAutoGC--) collectGarbage(board);
	generateNewChunks(board);
	bval (*change)[CHUNKSIZE2] = malloc(board->size * CHUNKSIZE2 * sizeof(bval));
	for(int i = 0; i < board->size; i++){
		calculateChunk(getChunk(board, i), change[i]);
	}
	for(int i = 0; i < board->size; i++){
		applyChange(getChunk(board, i), change[i]);
	}
	free(change);
	board->iterations++;
}

void iterateBoardTimes(struct board *board, int times){
	while(times--){
		iterateBoard(board);
	}
}

/*void collectGarbage(struct board *b){*/
	/*b->untilAutoGC = GCRUNFREQ;*/
	/*int marked[b->size];*/
	/*int markednum = 0;*/
	/*for(int i = 0; i < b->size; i++){*/
		/*struct chunk *c = getChunk(b, i);*/
		/*dprintf("GC: Testing Chunk %i " COORDINATES ":\n", i, c->locx, c->locy);*/
		/*if(chunkEmpty(c)){*/
			/*dprintf("GC: EMPTY!\n");*/
			/*markednum++;*/
			/*marked[i] = 1;*/
			/*//This chunk will be deleted so remove its links*/
			/*for(int j = 0; j < 8; j++){*/
				/*if(c->neighbors[j]){*/
					/*c->neighbors[j]->neighbors[neighborOpposite(j)] = NULL;*/
				/*}*/
			/*}*/
		/*}*/
	/*}*/
	/*if(marked[b->curChunk]) b->curChunk = -1;*/

	/*if(markednum < b->size){*/
		/*int first = 0;*/
		/*int last = b->size;*/
		/*while(1){*/
			/*while(first < last && !marked[first]) first++;*/
			/*while(last > first && marked[last]) last--;*/
			/*if(first >= last) break;*/
			/*if(b->curChunk == last){*/
				/*b->curChunk = first;*/
			/*}*/
			/*struct chunk *firstChunk = getChunk(b, first);*/
			/*memcpy(firstChunk, getChunk(b, last), sizeof(struct chunk));*/
			/*firstChunk->boardOffset = first;*/
			/*for(int i = 0; i < 8; i++){*/
				/*if(firstChunk->neighbors[i]){*/
					/*//make all neighbors have the new pointer*/
					/*firstChunk->neighbors[i]->neighbors[neighborOpposite(i)] = firstChunk;*/
				/*}*/
			/*}*/
			/*first++;*/
		/*}*/
		/*b->size -= markednum;*/
	/*}else{*/
		/*b->curChunk = -1;*/
		/*b->size = 0;*/
	/*}*/
	/*resizeBoardMin(b, b->size);*/
/*}*/

void collectGarbage(struct board *b){
	if(b->size == 0) return;
	int first = 0;
	int end = b->size - 1;
	for(;;){
		while(!chunkEmpty(getChunk(b, first))){
			first++;
		}
		while(chunkEmpty(getChunk(b, end))){
			end--;
		}
		if(first >= end) break;
		size_t diff = (end - first) * sizeof(struct chunk);
		memcpy(getChunk(b, first), getChunk(b, end), sizeof(struct chunk));
		for(int i = 0; i < 8; i++){
			getChunk(b, first)->neighbors[i] -= diff;
		}
	}
	b->untilAutoGC = GCRUNFREQ;
	resizeBoardMin(b, b->size);
}

int resizeBoardMin(struct board *b, int new){
	int newmax = DEFAULTWIDTH;
	while(newmax < new) newmax <<= 1;
	return resizeBoard(b, newmax);
}

int resizeBoard(struct board *b, int new){
	b->maxSize = new;
	struct chunk *oldplace = b->chunks; //this is probably super illegal
	b->chunks = realloc(b->chunks, new * sizeof(struct chunk));
	if(!b->chunks) return -1;
	int diff = b->chunks - oldplace;
	if(diff){
		for(int i = 0; i < b->size; i++){
			for(int k = 0; k < 8; k++){
				if(getChunk(b, i)->neighbors[k]){
					getChunk(b, i)->neighbors[k] += diff;
				}
			}
		}
	}
	return 0;
}

static int checkNeighborhood(
	struct chunk *restrict a,
	struct chunk *restrict b,
	int x, int y, int j, int k
){
	//x = b->locx
	//y = b->locy
	if ((x + j == a->locx) && (y + k == a->locy)){
		int del = neighborDelta(j, k);
		a->neighbors[neighborOpposite(del)] = b;
		b->neighbors[del] = a;
		return 1;
	}
	return 0;
}

void addChunk(struct board *b, int x, int y, const bval *mem){
	if(getChunkPos(b, x, y) >= 0) return; //exists
	if(b->size >= b->maxSize){
		resizeBoard(b, b->maxSize << 1);
	}

	struct chunk *n = nextChunk(b);

	if(mem){
		memcpy(n->board, mem, CHUNKSIZE2);
	}else{
		memset(n->board, 0, CHUNKSIZE2);
	}

	n->locx = x;
	n->locy = y;
	n->boardOffset = b->size;
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

void setBoardName(struct board *b, const char *name){
	/*cprintf("Board name changed to %s.", name);*/
	strcpy(b->name, name);
}

struct board *createBoard(const char *name){
	struct board *b = malloc(sizeof *b);
	b->chunks = NULL;
	b->untilAutoGC = GCRUNFREQ;
	if(name) setBoardName(b, name); else setBoardName(b, "empty");
	return b;
}

void initializeBoard(struct board *b){
	b->size = 0;
	resizeBoardMin(b, 0);
	b->iterations = 0;
	b->curChunk = -1;
}

void freeBoard(struct board *board){
	free(board->chunks);
	free(board);
}

int getChunkPos(const struct board *b, int x, int y){
	for(int i = 0; i < b->size; i++){
		if(getChunk(b, i)->locx == x && getChunk(b, i)->locy == y){
			return i;
		}
	}
	return -1;
}

int moveBoard(struct board *b, int x, int y){
	if(x == 0 && y == 0) return 0;
	if((x == -1 || x == 0 || x == 1) && (y == -1 || y == 0 || y == 1)){
		struct chunk *ch = curChunk(b)->neighbors[neighborDelta(x, y)];
		if(!ch) return -1;
		b->curChunk = ch->boardOffset;
		return 0;
	}
	return setBoard(b, curChunk(b)->locx + x, curChunk(b)->locy + y);
}

int setBoard(struct board *b, int x, int y){
	int pos = getChunkPos(b, x, y);
	if(pos < 0) return -1;
	b->curChunk = pos;
	return 0;
}

static void drawCross(const struct chunk *ch, int dir, int x, int y, char c){
	int hasneighbor = dir == NE_HERE ? 1 : ch->neighbors[dir] != NULL;
	if(USECOLOR){
		int col = hasneighbor ? COL_GREEN : COL_YELLOW;
		attron(COLOR_PAIR(col));
		mvaddch(CHUNKSIZE + y, x, c);
		attroff(COLOR_PAIR(col));
	}else{
		mvaddch(CHUNKSIZE + y, x, hasneighbor ? c : ' ');
	}
}

static void drawChunkData(const struct chunk *c){
	drawCross(c, NE_L,  0, 1, '-');
	drawCross(c, NE_R,  2, 1, '-');
	drawCross(c, NE_U,  1, 0, '|');
	drawCross(c, NE_D,  1, 2, '|');
	drawCross(c, NE_UR, 2, 0, '/');
	drawCross(c, NE_DR, 2, 2, '\\');
	drawCross(c, NE_UL, 0, 0, '\\');
	drawCross(c, NE_DL, 0, 2, '/');
	drawCross(c, NE_HERE, 1, 1, '*');
	mvprintw(CHUNKSIZE, 4, "At " COORDINATES,
		c->locx, c->locy
	);
}

static void drawBoardData(const struct board *b){
	mvprintw(CHUNKSIZE + 1, 4, "Iteration %i Index %i Size %i NextGC %i",
		b->iterations,
		b->curChunk,
		b->size,
		b->untilAutoGC
	);
	mvprintw(CHUNKSIZE + 2, 4, "Board '%s'",
		b->name
	);
}

void drawBoard(const struct board *b){
	if(b->curChunk > b->size){
		mvaddstr(1, 1, "Internal error.");
	}else if(b->size == 0){
		mvaddstr(1, 1, "This board is empty.");
		drawBoardData(b);
	}else if(b->curChunk == -1){
		mvaddstr(1, 1, "There is no currently selected chunk.");
		drawBoardData(b);
	}else{
		struct chunk *c = curChunk(b);
		drawChunk(c);
		drawChunkData(c);
		drawBoardData(b);
	}
}
