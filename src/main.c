#include "main.h"

//This sucks
//TODO make this not suck
static char gstr[GSTRINGSIZE * 2];
void setgStrings(const char *a, const char *b){
	memset(gstr, 0, GSTRINGSIZE * 2);
	strcpy(gstr, a);
	strcpy(gstr + GSTRINGSIZE, b);
}

int main(int argc, char **argv){
#ifdef DEBUG
	DEBUG_FILE = fopen("debug.txt", "w");
#endif
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	clear();
	start_color();
	init_pair(COL_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COL_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COL_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COL_WHITE, COLOR_WHITE, COLOR_BLACK);

	init_pair(COL_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COL_CYAN, COLOR_CYAN, COLOR_BLACK);

	USECOLOR = 1;

	setgStrings(
		"Welcome to the game of life! Press space/zxc to begin!",
		"Use arrows/wasd to move. Press q to quit."
	);

	char *PATTERN = NULL;
	int getoptval;
	while((getoptval = getopt(argc, argv, "cp:")) != -1){
		switch(getoptval)
		{
		//Disable color
		case 'c':
			USECOLOR = 0;
		break;
		case 'p':
			PATTERN = optarg;
		break;
		}
	}
	struct board *b = createBoard();
	addChunk(b, 0, 0);
	setBoard(b, 0, 0);
#define on(x, y) curChunk(b)->board[at(x, y)] = 1
	if(PATTERN){
		if(!strcmp(PATTERN, "glider")){
			on(1, 1);
			on(2, 2);
			on(3, 2);
			on(2, 3);
			on(3, 1);
		}
	}
#undef on

//START TESTS
	/*goto CLEANUP;*/
//END TESTS

	while(1){
		erase();
		drawBoard(b);
		mvaddnstr(CHUNKSIZE + 3, 0, gstr, GSTRINGSIZE);
		mvaddnstr(CHUNKSIZE + 4, 0, gstr + GSTRINGSIZE, GSTRINGSIZE);
		refresh();
		int in = input(b);
		if(in == IN_QUIT || in == IN_DONE) goto CLEANUP;
	}

	CLEANUP:
	freeBoard(b);
	endwin();
	printf("Program complete");
	return 0;
}
