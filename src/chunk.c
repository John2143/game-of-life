#include "chunk.h"

static void calculateColorMapFromChange(struct chunk *chunk, bval *change, colormap *map){
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
static const char *SURROUND = "012345678";

static void drawBuf(const bval *inbuf, const char *ascii, int offset){
	attron(COLOR_PAIR(COL_LIVE));
	for(int y = 0; y < CHUNKSIZE; y++){
		for(int x = 0; x < CHUNKSIZE; x++){
			mvaddch(x, y + offset, ascii[inbuf[at(x, y)]]);
		}
	}
	attroff(COLOR_PAIR(COL_LIVE));
}

static void drawBufColor(const bval *inbuf, const char *ascii, int offset, const colormap *cols){
	for(int y = 0; y < CHUNKSIZE; y++){
		for(int x = 0; x < CHUNKSIZE; x++){
			attron(COLOR_PAIR(cols[at(x, y)]));
			mvaddch(x, y + offset, ascii[inbuf[at(x, y)]]);
			attroff(COLOR_PAIR(cols[at(x, y)]));
		}
	}
}

void drawChunk(struct chunk *chunk){
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
static bval getv(struct chunk *chunk, int x, int y){
	if(!chunk) return 0;

	int dx = 0, dy = 0;
	if(x < 0){x = x + CHUNKSIZE; dx = -1;}
	else if(x >= CHUNKSIZE){x = x - CHUNKSIZE; dx = 1;}

	if(y < 0){y = y + CHUNKSIZE; dy = -1;}
	else if(y >= CHUNKSIZE){y = y - CHUNKSIZE; dy = 1;}

	int d = neighborDelta(dx, dy);
	switch(d){
		case NE_HERE: return chunk->board[at(x, y)];
		case NE_NONE: return 0;//TODO error here
		default: return getv(chunk->neighbors[d], x, y);
	}
}


void calculateChunk(struct chunk *chunk, bval *change){
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
void applyChange(struct chunk *chunk, bval *change){
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
