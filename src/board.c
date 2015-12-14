#include "board.h"

static int onEdge(const scp c, int i, int delta, int times){
	while(times--){
		if(c->board[i]) return 1;
		i += delta;
	}
	return 0;
}

int generateNewChunks(struct board *b){
	int size = b->size;
	forEachChunk(b, c){
		int x = c->locx, y = c->locy;
		if(onEdge(c, at_TR, CHUNKSIZE, CHUNKSIZE)){ //Right
			addChunk(b, x + 1,     y, NULL);
		}
		if(onEdge(c, at_TL, CHUNKSIZE, CHUNKSIZE)){ //Left
			addChunk(b, x - 1,     y, NULL);
		}
		if(onEdge(c, at_BL,         1, CHUNKSIZE)){ //Down
			addChunk(b,     x, y + 1, NULL);
		}
		if(onEdge(c, at_TL,         1, CHUNKSIZE)){ //Up
			addChunk(b,     x, y - 1, NULL);
		}
	}
	return b->size - size;
}

void iterateBoard(struct board *board){
	if(!board->untilAutoGC--) collectGarbage(board);
	generateNewChunks(board);
	bval (*change)[CHUNKSIZE2] = malloc(board->size * CHUNKSIZE2 * sizeof(bval));
	int i = 0;
	forEachChunk(board, p){
		calculateChunk(p, change[i++]);
	}
	i = 0;
	forEachChunk(board, p){
		applyChange(p, change[i++]);
	}
	free(change);
	board->iterations++;
}

void iterateBoardTimes(struct board *board, int times){
	while(times--){
		iterateBoard(board);
	}
}

void collectGarbage(struct board *b){
	if(b->size == 0) return;
	scp p = b->first, next;
	while(p){
		next = p->next;
		if(chunkEmpty(p)){
			if(b->curChunk == p) b->curChunk = NULL;
			removeChunk(b, p);
		}
		p = next;
	}

	b->untilAutoGC = GCRUNFREQ;
}

static int checkNeighborhood(
	scp restrict a,
	scp restrict b,
	int bx, int by, int j, int k
){
	//x = b->locx
	//y = b->locy
	int ax = a->locx, ay = a->locy;
	if ((bx + j == ax) && (by + k == ay)){
		int del = neighborDelta(j, k);
		b->neighbors[del] = a;
		a->neighbors[neighborOpposite(del)] = b;
		return 1;
	}
	return 0;
}

void removeChunk(struct board *b, scp c){
	if(b->first == c) b->first = c->next;

	if(c->next) c->next->last = c->last;
	if(c->last) c->last->next = c->next;

	for(int j = 0; j < 8; j++){
		if(c->neighbors[j]){
			c->neighbors[j]->neighbors[neighborOpposite(j)] = NULL;
		}
	}

	free(c);
	b->size--;
}

void addChunk(struct board *b, int x, int y, const bval *mem){
	if(getChunkAtPos(b, x, y) != NULL) return; //exists

	scp next = malloc(sizeof(struct chunk));

	next->locx = x;
	next->locy = y;

	next->last = NULL;
	next->next = b->first;
	b->first = next;


	if(mem){
		memcpy(next->board, mem, CHUNKSIZE2);
	}else{
		memset(next->board, 0, CHUNKSIZE2);
	}

	for(int i = 0; i < 8; i++){
		next->neighbors[i] = NULL;
	}

	forEachChunk(b, p){
		#define X(j, k) \
		if(checkNeighborhood(p, next, x, y, j, k)) continue;
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
	b->first = NULL;
	b->untilAutoGC = GCRUNFREQ;
	if(name) setBoardName(b, name); else setBoardName(b, "empty");
	return b;
}

void initializeBoard(struct board *b){
	b->size = 0;
	b->iterations = 0;
	b->curChunk = NULL;
}

void emptyBoard(struct board *board){
	scp p = board->first, next;
	while(p){
		next = p->next;
		free(p);
		p = next;
	}
}

void freeBoard(struct board *board){
	emptyBoard(board);
	free(board);
}

scp getChunkAtPos(const struct board *b, int x, int y){
	forEachChunk(b, p){
		if(p->locx == x && p->locy == y){
			return p;
		}
	}
	return NULL;
}

int moveBoard(struct board *b, int x, int y){
	if(x == 0 && y == 0) return 0;
	if((x == -1 || x == 0 || x == 1) && (y == -1 || y == 0 || y == 1)){
		scp ch = b->curChunk->neighbors[neighborDelta(x, y)];
		if(!ch) return -1;
		b->curChunk = ch;
		return 0;
	}
	return setBoard(b, b->curChunk->locx + x, b->curChunk->locy + y);
}

int setBoard(struct board *b, int x, int y){
	scp p = getChunkAtPos(b, x, y);
	if(p == NULL) return -1;
	b->curChunk = p;
	return 0;
}

static void drawCross(const scp ch, int dir, int x, int y, char c){
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

static void drawChunkData(const scp c){
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
	mvprintw(CHUNKSIZE + 1, 4, "Iteration %i Size %i NextGC %i",
		b->iterations,
		b->size,
		b->untilAutoGC
	);
	mvprintw(CHUNKSIZE + 2, 4, "Board '%s'",
		b->name
	);
}

void drawBoard(const struct board *b){
	if(b->size == 0){
		mvaddstr(1, 1, "This board is empty.");
	}else if(b->curChunk == NULL){
		mvaddstr(1, 1, "There is no currently selected chunk.");
	}else{
		drawChunk(b->curChunk);
		drawChunkData(b->curChunk);
	}
	drawBoardData(b);
}
