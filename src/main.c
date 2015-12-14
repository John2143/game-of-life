#include "main.h"

static int conline;
static char (*consoleBuffer)[GSTRINGSIZE];
void cprintf(const char *format, ...){
	va_list args;
	va_start(args, format);
	//GSTRING - 1 will ensure that the console will always end in a null terminator
	//  because this is called with calloc.
	vsnprintf((consoleBuffer[conline++ % CONSIZE]), GSTRINGSIZE-1, format, args);
	va_end(args);
}

void consoleRenderer(void){
	for(int j = 0; j < CONSIZE; j++){
		mvaddstr(CHUNKSIZE + 5 + j, 0, consoleBuffer[(j + conline) % CONSIZE]);
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
	init_pair(COL_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COL_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(COL_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COL_WHITE, COLOR_WHITE, COLOR_BLACK);

	init_pair(COL_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COL_CYAN, COLOR_CYAN, COLOR_BLACK);

	WINDOWMAX_Y = getmaxy(stdscr);
	WINDOWMAX_X = getmaxx(stdscr);

	INPUTMODE = INM_NORMAL;

	dprintf("Window size is %ix%i\n", WINDOWMAX_X, WINDOWMAX_Y);

	USECOLOR = 1;
	GCRUNFREQ = 50;
	CONSIZE = WINDOWMAX_Y - CHUNKSIZE - 5;

	const char *PATTERN = NULL;
	const char *BOARDNAME = NULL;
	int getoptval;
	while((getoptval = getopt(argc, argv, "cg:p:n:s:")) != -1){
		switch(getoptval)
		{
		//Disable color
		case 'c':
			USECOLOR = 0;
		break;
		case 'p':
			PATTERN = optarg;
		break;
		case 'n':
			BOARDNAME = optarg;
		break;
		case 'g':
			GCRUNFREQ = atoi(optarg);
		break;
		case 's':
			CONSIZE = atoi(optarg);
		break;
		}
	}

	CONSIZE = clamp(CONSIZE, 5, 15);
	consoleBuffer = calloc(CONSIZE * GSTRINGSIZE, 1);

	cprintf("Welcome to the game of life! Press space/zxc to begin!");
	cprintf("Use arrows/wasd to move. Press q to quit.");

	struct board *b;
	if(PATTERN){
		b = readNewBoard(PATTERN);
		if(BOARDNAME){
			setBoardName(b, BOARDNAME);
		}
	}else{
		b = createBoard(BOARDNAME);
		initializeBoard(b);
	}

//START TESTS
/*#define on(x, y) curChunk(b)->board[at(x, y)] = 1*/
	/*on(1, 1);*/
	/*on(2, 2);*/
	/*on(3, 2);*/
	/*on(2, 3);*/
	/*on(3, 1);*/
	/*goto CLEANUP;*/
//END TESTS

	while(1){
		erase();
		drawBoard(b);
		inputRenderer();
		consoleRenderer();
		refresh();
		if(!input(b)) goto CLEANUP;
	}

	CLEANUP:
#ifdef DEBUG
	fclose(DEBUG_FILE);
#endif
	free(consoleBuffer);
	freeBoard(b);
	endwin();
	printf("Program complete");
	return 0;
}
