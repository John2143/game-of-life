#include "chunk.h"

static int onEdge(const struct chunk *c, int i, int delta, int times){
	while(times--){
		if(c->board[i]) return 1;
		i += delta;
	}
	return 0;
}

int newChunks(const struct chunk *c){
	return
		onEdge(c, at_TR, CHUNKSIZE, CHUNKSIZE) | //Right
		onEdge(c, at_TL, CHUNKSIZE, CHUNKSIZE) << 1 | //Left
		onEdge(c, at_BL, 1, CHUNKSIZE) << 2 | //Down
		onEdge(c, at_TL, 1, CHUNKSIZE) << 3; //Up
}

static void calculateColorMapFromChange(const struct chunk *chunk, bval *change, colormap *map){
	for(int i = 0; i < CHUNKSIZE2; i++){
		if(chunk->board[i]){
			if(change[i] < 2 || change[i] > 3){
				map[i] = COL_DIE;
			}else{
				map[i] = COL_LIVE;
			}
		}else{
			if(change[i] == 3){
				map[i] = COL_SPAWN;
			}else{
				map[i] = COL_DEAD;
			}
		}
	}
}

//Used to display the playing field. 0 is -, 1 is *
static const char *ASCII = "-*";
//Used to display the delta field. This is just faster than printf.
static const char *SURROUND = "0123456789";

static void drawBuf(const bval *inbuf, const char *ascii, int offset){
	attron(COLOR_PAIR(COL_LIVE));
	int x = 0;
	int y = 0;
	for(int i = 0; i < CHUNKSIZE2; i++){
		mvaddch(y, x + offset, ascii[inbuf[i]]);
		if(++x == CHUNKSIZE){
			y++;
			x = 0;
		}
	}
	attroff(COLOR_PAIR(COL_LIVE));
}

static void drawBufColor(const bval *inbuf, const char *ascii, int offset, const colormap *cols){
	int x = 0;
	int y = 0;
	for(int i = 0; i < CHUNKSIZE2; i++){
		attron(COLOR_PAIR(cols[i]));
		mvaddch(y, x + offset, ascii[inbuf[i]]);
		attroff(COLOR_PAIR(cols[i]));
		if(++x == CHUNKSIZE){
			y++;
			x = 0;
		}
	}
}

void drawChunk(const struct chunk *chunk){
	drawBuf(chunk->board, ASCII, 0);

	bval change[CHUNKSIZE2];
	calculateChunk(chunk, change);

	if(USECOLOR){
		colormap colchange[CHUNKSIZE2];
		calculateColorMapFromChange(chunk, change, colchange);
		drawBufColor(change, SURROUND, CHUNKSIZE + 1, colchange);
	}else{
		drawBuf(change, SURROUND, CHUNKSIZE + 1);
	}
}

int neighborDelta(int x, int y){
	if(x == 0){
		if(y == 0) return NE_HERE;
		else if(y == 1) return NE_D;
		else return NE_U;
	}else if(x == 1){
		if(y == 0) return NE_R;
		else if(y == 1) return NE_DR;
		else return NE_UR;
	}else{
		if(y == 0) return NE_L;
		else if(y == 1) return NE_DL;
		else return NE_UL;
	}
}

int neighborOpposite(int n){
	switch(n){
	case NE_R: return NE_L;
	case NE_L: return NE_R;
	case NE_U: return NE_D;
	case NE_D: return NE_U;
	case NE_UR: return NE_DL;
	case NE_DR: return NE_UL;
	case NE_UL: return NE_DR;
	case NE_DL: return NE_UR;
	case NE_HERE: return NE_HERE;
	default: return NE_NONE;
	}
}

//Doesnt work if its more than one from the edge
/*static bval getv(const struct chunk *chunk, int x, int y){*/
	/*if(!chunk) return 0;*/

	/*int dx = 0, dy = 0;*/
	/*if(x < 0){x = x + CHUNKSIZE; dx = -1;}*/
	/*else if(x >= CHUNKSIZE){x = x - CHUNKSIZE; dx = 1;}*/

	/*if(y < 0){y = y + CHUNKSIZE; dy = -1;}*/
	/*else if(y >= CHUNKSIZE){y = y - CHUNKSIZE; dy = 1;}*/

	/*int d = neighborDelta(dx, dy);*/
	/*switch(d){*/
		/*case NE_HERE: return chunk->board[at(x, y)];*/
		/*case NE_NONE: return 0;//TODO error here*/
		/*default: return getv(chunk->neighbors[d], x, y);*/
	/*}*/
/*}*/

/*void calculateChunk(const struct chunk *chunk, bval *change){*/
	/*int v, ix, iy;*/
	/*for(ix = 0; ix < CHUNKSIZE; ix++){*/
		/*for(iy = 0; iy < CHUNKSIZE; iy++){*/
			/*v = 0;*/
			/*#define X(j, k) if(getv(chunk, ix + j, iy + k)) v++;*/
			/*EXPAND_DIRS*/
			/*#undef X*/
			/*change[at(ix, iy)] = v;*/
		/*}*/
	/*}*/
/*}*/

#define CHUNKSIZE_MASK (CHUNKSIZE - 1)
#define isOnBorderLeft(i) (!(i & CHUNKSIZE_MASK))
#define isOnBorderRight(i) (isOnBorderRight(i + 1))
#define isOnBorderTop(i) (!(i & ~CHUNKSIZE_MASK))
#define isOnBorderBottom(i) (i - CHUNKSIZE2 < 0)

static int isOnBorder(int i){
	return
		isOnBorderRight(i) ||
		isOnBorderLeft(i) ||
		isOnBorderTop(i) ||
		isOnBorderBottom(i);
}

static bval getv(const struct chunk *chunk, int x, int y){
	if(!chunk) return 0;

	int dx = 0, dy = 0;
	if(x == -1) return 0;
	if(x == CHUNKSIZE) return 0;
	if(y == -1) return 0;
	if(y == CHUNKSIZE) return 0;

	switch(d){
		case NE_HERE: return chunk->board[at(x, y)];
		case NE_NONE: return 0;//TODO error here
		default: return getv(chunk->neighbors[d], x, y);
	}
}

static int edgeDelta(
	const struct chunk *c, bval *change,
	int boardi, int changei, int delta
){
	int times = CHUNKSIZE;
	while(times--){
		if(c->board[boardi]){
			//MAKE SURE TO NOT GO OUT OF BOUNDS HERE
		}
		boardi += delta;
		changei += delta;
	}
	return 0;
}

void calculateChunk(const struct chunk *ch, bval *change){
	
	//TODO STUFF HERE

	edgeDelta( //top edge
		ch->neighbors[NE_U], change,
		at_BL, at_TL, 1
	);
	edgeDelta( //bottom edge
		ch->neighbors[NE_D], change,
		at_TL, at_BL, 1
	);
	edgeDelta( //left edge
		ch->neighbors[NE_L], change,
		at_TR, at_TL, CHUNKSIZE
	);
	edgeDelta( //right edge
		ch->neighbors[NE_R], change,
		at_TL, at_TR, CHUNKSIZE
	);

	if(ch->neighbors[NE_UL]){
		if(ch->neighbors[NE_UL]->board[at_BR]){
			change[at_TL]++;
		}
	}
	if(ch->neighbors[NE_UR]){
		if(ch->neighbors[NE_UR]->board[at_BL]){
			change[at_TR]++;
		}
	}
	if(ch->neighbors[NE_BL]){
		if(ch->neighbors[NE_BL]->board[at_TR]){
			change[at_BL]++;
		}
	}
	if(ch->neighbors[NE_BR]){
		if(ch->neighbors[NE_BR]->board[at_TL]){
			change[at_BR]++;
		}
	}
}

/*Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 *Any live cell with two or three live neighbours lives on to the next generation.
 *Any live cell with more than three live neighbours dies, as if by over-population.
 *Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */
void applyChange(struct chunk *chunk, const bval *change){
	for(int i = 0; i < CHUNKSIZE2; i++){
		if(chunk->board[i]){
			if(change[i] < 2 || change[i] > 3){
				chunk->board[i] = 0;
			}
		}else{
			if(change[i] == 3){
				chunk->board[i] = 1;
			}
		}
	}
}
