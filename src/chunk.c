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

int chunkEmpty(const struct chunk *chunk){
	int i = CHUNKSIZE2;
	while(i--){
		if(chunk->board[i]) return 0;
	}
	return 1;
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
static const char *ASCII = ".*";
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
	bval change[CHUNKSIZE2];
	calculateChunk(chunk, change);

	if(USECOLOR){
		colormap colchange[CHUNKSIZE2];
		calculateColorMapFromChange(chunk, change, colchange);
		drawBufColor(change, SURROUND, CHUNKSIZE + 1, colchange);
		drawBufColor(chunk->board, ASCII, 0, colchange);
	}else{
		drawBuf(change, SURROUND, CHUNKSIZE + 1);
		drawBuf(chunk->board, ASCII, 0);
	}
}

int neighborDelta(int x, int y){
	switch(x){
	case 0:
		switch(y){
		case  0: return NE_HERE;
		case  1: return NE_D;
		case -1: return NE_U;
		}
	case 1:
		switch(y){
		case  0: return NE_R;
		case  1: return NE_DR;
		case -1: return NE_UR;
		}
	default:
		switch(y){
		case  0: return NE_L;
		case  1: return NE_DL;
		case -1: return NE_UL;
		}
	}
	return NE_NONE;
}

//Doesnt work if its more than one from the edge
static bval getv(const struct chunk *chunk, int x, int y){
	if(!chunk) return 0;
	int dx = 0, dy = 0;
	if(x == -1){x = CHUNKSIZE - 1; dx--;}
	else if(x == CHUNKSIZE){x = 0; dx++;}
	if(y == -1){y = CHUNKSIZE - 1; dy--;}
	else if(y == CHUNKSIZE){y = 0; dy++;}
	int d = neighborDelta(dx, dy);
	switch(d){
		case NE_HERE: return chunk->board[at(x, y)];
		default: return getv(chunk->neighbors[d], x, y);
	}
}


void calculateChunk(const struct chunk *chunk, bval *change){
	int v, ix, iy;
	for(ix = 0; ix < CHUNKSIZE; ix++){
		for(iy = 0; iy < CHUNKSIZE; iy++){
			v = 0;
			#define X(j, k) if(getv(chunk, ix + j, iy + k)) v++;
			EXPAND_DIRS
			#undef X
			change[at(ix, iy)] = v;
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
