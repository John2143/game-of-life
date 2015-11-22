#include "main.h"

#include "chunk.h"
#include "board.h"

void drawBuf(const bval *inbuf, const char *ascii){
	char buf[CHUNKSIZE2 + CHUNKSIZE];
	int bp = 0;
	int i = 0;
	int charleft = CHUNKSIZE;
	while(i < CHUNKSIZE2){
		buf[bp++] = ascii[inbuf[i++]];
		if(!--charleft){
			buf[bp++] = '\n';
			charleft = CHUNKSIZE;
		}
	}
	buf[sizeof(buf)] = '\0';
	printf("%s", buf);
}

int main(void){
	struct board *b = createBoard();
	addChunk(b, 0, 0);
	struct chunk c = b->chunks[0];
	c.board[at(0, 1)] = 1;
	c.board[at(1, 2)] = 1;
	c.board[at(2, 2)] = 1;
	c.board[at(2, 1)] = 1;
	c.board[at(2, 0)] = 1;
	for(int i = 0; i < 5; i++){
		drawChunk(&c);
		calculateChunk(&c);
	}
	freeBoard(b);
	return 0;
}
