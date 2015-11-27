#include "main.h"

//This sucks
//TODO make this not suck
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
#ifdef DEBUG
	DEBUG_FILE = fopen("debug.txt", "w");
#endif
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

	char *PATTERN = NULL;
	int getoptval;
	while((getoptval = getopt (argc, argv, "cp:")) != -1){
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
#define on(x, y) curChunk(b).board[at(x, y)] = 1
	if(PATTERN){
		if(!strcmp(PATTERN, "glider")){
			on(0, 0);
			on(1, 1);
			on(1, 2);
			on(2, 1);
			on(2, 0);
		}
	}
#undef on

//START TESTS
//END TESTS

	while(1){
		drawBoard(b);
		mvaddnstr(CHUNKSIZE + 3, 0, gstr, GSTRINGSIZE);
		mvaddnstr(CHUNKSIZE + 4, 0, gstr + GSTRINGSIZE, GSTRINGSIZE);
		refresh();
		if(!input(b)) goto CLEANUP;
	}

	CLEANUP:
	freeBoard(b);
	endwin();
	printf("Program complete");
	return 0;
}
