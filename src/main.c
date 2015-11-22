#include "main.h"

#include "board.h"

void drawBuf(const bval *inbuf, const char *ascii, int offset){
#ifdef NOCURSES
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
	(void) offset;
#else
	for(int y = 0; y < CHUNKSIZE; y++){
		for(int x = 0; x < CHUNKSIZE; x++){
			mvaddch(x + offset, y, ascii[inbuf[at(x, y)]]);
		}
	}
#endif
}

#ifdef NOCURSES
	#define initscr() ;
	#define refresh() ;
	#define endwin() ;
#endif

int main(void){
	initscr();
	struct board *b = createBoard();

	addChunk(b, 0, 0);
	setBoard(b, 0, 0);
	curChunk(b).board[at(0, 1)] = 1;
	curChunk(b).board[at(1, 2)] = 1;
	curChunk(b).board[at(2, 2)] = 1;
	curChunk(b).board[at(2, 1)] = 1;
	curChunk(b).board[at(2, 0)] = 1;

	while(1){
		drawBoard(b);
		iterateBoard(b);
		refresh();
		switch(getch()){
			case 'q':
				goto CLEANUP;
			case 'w':
				break;
			case 's':
				break;
			case 'a':
				break;
			case 'd':
				break;
			default:
				break;
		}
	}
	CLEANUP:

	freeBoard(b);
	endwin();
	printf("Program complete");
	return 0;
}
