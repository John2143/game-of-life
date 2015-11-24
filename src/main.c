#include "main.h"

#include "board.h"

const char *ASCII = "-*";
const char *SURROUND = "012345678";

void drawBuf(const bval *inbuf, const char *ascii, int offset){
	attron(COLOR_PAIR(COL_LIVE));
	for(int y = 0; y < CHUNKSIZE; y++){
		for(int x = 0; x < CHUNKSIZE; x++){
			mvaddch(x, y + offset, ascii[inbuf[at(x, y)]]);
		}
	}
	attroff(COLOR_PAIR(COL_LIVE));
}

void drawBufColor(const bval *inbuf, const char *ascii, int offset, const colormap *cols){
	for(int y = 0; y < CHUNKSIZE; y++){
		for(int x = 0; x < CHUNKSIZE; x++){
			attron(COLOR_PAIR(cols[at(x, y)]));
			mvaddch(x, y + offset, ascii[inbuf[at(x, y)]]);
			attroff(COLOR_PAIR(cols[at(x, y)]));
		}
	}
}

#define IN_MASK_BITS 4
#define IN_MASK ((1 << IN_MASK_BITS) - 1)

#define IN_QUIT 0
#define IN_ITERATE 1
#define IN_NOOP 2

//TODO allow moving into the void to go somewhere else
static int moveBoardHandler(struct board *b, int x, int y){
	if(moveBoard(b, x, y) < 0){
		char buf[GSTRINGSIZE];
		sprintf(buf, "%i, %i is empty.", curChunk(b).locx + x, curChunk(b).locy + y);
		setgStrings("You cannot move the board in that direction", buf);
	}
	return IN_NOOP;
}

static int getInput(struct board *b){
	switch(getch()){
	case 'q':
		return IN_QUIT; //end
	case 'w':
	case KEY_UP:
		return moveBoardHandler(b, 0, -1);
	case 's':
	case KEY_DOWN:
		return moveBoardHandler(b, 0, 1);
	case 'a':
	case KEY_LEFT:
		return moveBoardHandler(b, -1, 0);
	case 'd':
	case KEY_RIGHT:
		return moveBoardHandler(b, 1, 0);
	/*case 'i':*/
		/*move(CHUNKSIZE + 6, 0);*/
		/*printw("Enter a number: ");*/
		/*refresh();*/
		/*char in[GSTRINGSIZE];*/
		/*if(fgets(in, GSTRINGSIZE, stdin) == NULL) return IN_NOOP;*/
		/*int v = atoi(in);*/
		/*if(v <= 0) return IN_NOOP;*/
		/*return IN_ITERATE | v << IN_MASK_BITS;*/
	case ' ':
		setgStrings(
			"Iterating...",
			""
		);
		return IN_ITERATE | 1 << IN_MASK_BITS;
	case 'z':
		setgStrings(
			"Iterating 5...",
			""
		);
		return IN_ITERATE | 5 << IN_MASK_BITS;
	case 'x':
		setgStrings(
			"Iterating 10...",
			""
		);
		return IN_ITERATE | 10 << IN_MASK_BITS;
	case 'c':
		setgStrings(
			"Iterating 100...",
			""
		);
		return IN_ITERATE | 100 << IN_MASK_BITS;
	default:
		setgStrings("Invalid move.", "");
		return IN_NOOP;
	}
}

static char gstr[GSTRINGSIZE * 2];
void setgStrings(const char *a, const char *b){
	memset(gstr, 0, GSTRINGSIZE * 2);
	strcpy(gstr, a);
	strcpy(gstr + GSTRINGSIZE, b);

	int i = GSTRINGSIZE * 2;
	while(--i){
		if(gstr[i] == '\0'){
			gstr[i] = ' ';
		}
	}
}

int main(int argc, char **argv){
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	clear();
	start_color();
	init_pair(COL_SPAWN, COLOR_BLUE, COLOR_BLACK);
	init_pair(COL_DIE, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COL_DEAD, COLOR_RED, COLOR_BLACK);
	init_pair(COL_LIVE, COLOR_WHITE, COLOR_BLACK);

	USECOLOR = 1;

	setgStrings(
		"Welcome to the game of life! Press space/zxc to begin!",
		"Use arrows/wasd to move. Press q to quit."
	);

	{
	int c;
	while((c = getopt (argc, argv, "c")) != -1){
		switch(c)
		{
		//Disable color
		case 'c':
			USECOLOR = 0;
		break;
		}
	}
	}
	struct board *b = createBoard();

	addChunk(b, 0, 0);
	setBoard(b, 0, 0);
	curChunk(b).board[at(0, 0)] = 1;
	curChunk(b).board[at(1, 1)] = 1;
	curChunk(b).board[at(1, 2)] = 1;
	curChunk(b).board[at(2, 1)] = 1;
	curChunk(b).board[at(2, 0)] = 1;

	while(1){
		drawBoard(b);
		mvaddnstr(CHUNKSIZE + 3, 0, gstr, GSTRINGSIZE);
		mvaddnstr(CHUNKSIZE + 4, 0, gstr + GSTRINGSIZE, GSTRINGSIZE);
		refresh();
		int in = getInput(b);
		switch(in & IN_MASK){
		case IN_QUIT: goto CLEANUP;
		case IN_ITERATE:
			in >>= IN_MASK_BITS;
			while(in--){
				iterateBoard(b);
			}
			break;
		case IN_NOOP:
			break;
		default:
			setgStrings("INTERNAL ERROR", "!!!!!!!!!!!!!!!");
		}
	}

	CLEANUP:
	freeBoard(b);
	endwin();
	printf("Program complete");
	return 0;
}
