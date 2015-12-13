#include <input.h>

static const char *INPUTMODENAMES[] = {"NORMAL", "DRAWING", "INPUT"};
const char *getInputModeName(void){
	return INPUTMODENAMES[INPUTMODE];
}

#define INPUTMAX 64
static char inputBuffer[INPUTMAX];
static int inputLength = 0;
//Implied inputLength and inputBuffer params
typedef void (*onFinishInput)(struct board *b, int len, const char *input);
static onFinishInput cbFinish;

static enum INM prevMode;
static void getInput(onFinishInput cb){
	prevMode = INPUTMODE;
	INPUTMODE = INM_TYPE;
	cbFinish = cb;
	inputLength = 0;
	memset(inputBuffer, 0, INPUTMAX);
}

static void renameBoardCB(struct board *b, int len, const char *input){
	(void) len;
	strncpysafe(b->name, input, 32);
	dprintf("AT RENAME");
}

static void normalHelp(){
	cprintf("-----------Normal mode help------------");
	cprintf("        WASD/Arrow keys to move        ");
	cprintf("     Space ZXCV to iterate forward     ");
	cprintf("    Q - Quit; G - Garbage Collector    ");
	cprintf("    J - Read; K - Write; R - Rename    ");
	cprintf("       ? - Help; ESC/Enter- Draw       ");
	cprintf("---------------------------------------");
}

static inline int getInputNORM(struct board *b, int c){
	switch(c){
	case 'w': case KEY_UP:    moveBoard(b,  0, -1); break;
	case 's': case KEY_DOWN:  moveBoard(b,  0,  1); break;
	case 'a': case KEY_LEFT:  moveBoard(b, -1,  0); break;
	case 'd': case KEY_RIGHT: moveBoard(b,  1,  0); break;

	case ' ': iterateBoardTimes(b, 1); break;
	case 'z': iterateBoardTimes(b, 5); break;
	case 'x': iterateBoardTimes(b, 10); break;
	case 'c': iterateBoardTimes(b, 100); break;
	case 'v': iterateBoardTimes(b, 1000); break;

	case 'q': return 0; //Quit
	case 'g': cprintf("Running GC..."); collectGarbage(b); break;
	case 'f': cprintf("Chosen random chunk"); b->curChunk = 0; break;
	case 'j': cprintf("Reading board..."); readBoard(b); break;
	case 'k': cprintf("Writing board..."); writeBoard(b); break;
	case 'r': getInput(renameBoardCB); break;
	case '\n': case 27: INPUTMODE = INM_DRAW; break;
	case '?': normalHelp(); break;
	}

	return 1;
}

static int drawX, drawY;
#define RINGMASK (CHUNKSIZE - 1)
#define RINGVAL(i) (RINGMASK & i)

#define MOVEBUF(draw, chk, chdx, chdy) \
	if(RINGVAL(draw) == chk) {\
		addChunk( \
			b, \
			curChunk(b)->locx + chdx, \
			curChunk(b)->locy + chdy, \
			NULL \
		); /* Will fail if the chunk exists*/ \
		moveBoard(b, chdx, chdy); \
	}

static void drawingHelp(){
	cprintf("----------Drawing mode help------------");
	cprintf("        WASD/Arrow keys to move        ");
	cprintf("          Z - Draw; X - Erase          ");
	cprintf("      ? - Help; ESC/Enter - Normal     ");
	cprintf("---------------------------------------");
}

static inline int getInputDRAW(struct board *b, int c){
	switch(c){
	case 'w': case KEY_UP:    MOVEBUF(--drawY, RINGMASK,  0, -1); break;
	case 's': case KEY_DOWN:  MOVEBUF(++drawY,        0,  0,  1); break;
	case 'a': case KEY_LEFT:  MOVEBUF(--drawX, RINGMASK, -1,  0); break;
	case 'd': case KEY_RIGHT: MOVEBUF(++drawX,        0,  1,  0); break;
	case 'z': curChunk(b)->board[at(drawX, drawY)] = 1; break;
	case 'x': curChunk(b)->board[at(drawX, drawY)] = 0; break;
	case '\n': case 27: INPUTMODE = INM_NORMAL; break;
	case '?': drawingHelp(); break;
	}
	return 1;
}

void inputRenderer(void){
	mvaddstr(CHUNKSIZE + 3, 0, getInputModeName());
	switch(INPUTMODE){
	case INM_TYPE:;
		int len = strlen(getInputModeName());
		char b[4];
		sprintf(b, "%i", inputLength);
		mvaddstr(CHUNKSIZE + 3, len + 1, inputBuffer);
		mvaddstr(CHUNKSIZE + 3, len + 2 + inputLength, b);
	break;
	case INM_DRAW:
		mvaddch(RINGVAL(drawY), RINGVAL(drawX), '%');
	break;
	case INM_NORMAL:
	break;
	}
}

static inline int getInputTYPE(struct board *b, int c){
	switch(c){
	case KEY_BACKSPACE:
	case '\b':
		if(inputLength)	inputBuffer[--inputLength] = '\0';
	break;
	case KEY_ENTER:
	case '\n':
		cbFinish(b, inputLength, inputBuffer);
	case 27:
		INPUTMODE = prevMode;
	break;
	default:
		if(inputLength < INPUTMAX) inputBuffer[inputLength++] = c;
	}
	return 1;
}

int input(struct board *b){
	int c = getch();
	switch(INPUTMODE){
		case INM_NORMAL: return getInputNORM(b, c);
		case INM_DRAW: return getInputDRAW(b, c);
		case INM_TYPE: return getInputTYPE(b, c);
	}
	return 1;//TODO error
}
