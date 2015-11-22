#include "chunk.h"

const char *ASCII = "-*";
const char *SURROUND = "012345678";

void drawChunk(struct chunk *chunk){
	drawBuf(chunk->board, ASCII);
}
bval getv(struct chunk *chunk, int x, int y){
	if(x >= 0 && y >= 0 && x < CHUNKSIZE && y < CHUNKSIZE)
		return chunk->board[at(x, y)];
	return 0;
}
#define expand(j, k) if(getv(chunk, ix + j, iy + k)) v++;
#define loop \
	for(ix = 0; ix < CHUNKSIZE; ix++){\
	for(iy = 0; iy < CHUNKSIZE; iy++){\
	v = 0;\
	i = at(ix, iy);
#define endloop }}

void calculateChunk(struct chunk *chunk){
	bval change[CHUNKSIZE2];
	int v, ix, iy, i;
	loop
		expand(-1, -1);
		expand(0, -1);
		expand(1, -1);
		expand(-1, 0);
		expand(1, 0);
		expand(-1, 1);
		expand(0, 1);
		expand(1, 1);
		change[i] = v;
	endloop

	drawBuf(change, SURROUND);

	loop
/*Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 *Any live cell with two or three live neighbours lives on to the next generation.
 *Any live cell with more than three live neighbours dies, as if by over-population.
 *Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 */
		if(chunk->board[i]){
			if(change[i] < 2 || change[i] > 3){
				chunk->board[i] = 0;
			}
		}else{
			if(change[i] == 3){
				chunk->board[i] = 1;
			}
		}
	endloop
}
#undef loop
#undef endloop
#undef expand
void iterateChunk(struct chunk *chunk){
	(void) chunk;
}
