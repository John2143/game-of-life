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
	setInputMode(INM_TYPE);
	cbFinish = cb;
	inputLength = 0;
	*inputBuffer = '\0';
}

static void renameBoardCB(struct board *b, int len, const char *input){
	(void) len;
	strncpysafe(b->name, input, BOARDNAMELENGTH);
}

#define SHAREDKEYSPRINT \
	cprintf("    J - Read; K - Write; R - Rename    ");

static void normalHelp(){
	cprintf("-----------Normal mode help------------");
	cprintf("        WASD/Arrow keys to move        ");
	cprintf("     Space ZXCV to iterate forward     ");
	cprintf("    Q - Quit; G - Garbage Collector    ");
	SHAREDKEYSPRINT
	cprintf("       ? - Help; ESC/Enter- Draw       ");
	cprintf("---------------------------------------");
}

#define SHAREDKEYS \
	case 'j': cprintf("Reading board..."); readBoard(b); break; \
	case 'k': cprintf("Writing board..."); writeBoard(b); break; \
	case 'r': getInput(renameBoardCB); break; \

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
	case 'f': cprintf("Chosen random chunk"); b->curChunk = b->first; break;
	case '\n': case 27:
		if(b->size == 0) addChunk(b, 0, 0, NULL);
		if(b->curChunk == NULL) b->curChunk = b->first;
		setInputMode(INM_DRAW);
	break;
	case '?': normalHelp(); break;

	SHAREDKEYS
	}

	return 1;
}

static int drawX, drawY;
#define RINGMASK (CHUNKSIZE - 1)
#define RINGVAL(i) (RINGMASK & (i))

#define MOVEBUF(op, draw, chk, chdx, chdy) \
	draw = RINGVAL(draw op 1); \
	if(draw == chk) {\
		addChunk( \
			b, \
			b->curChunk->locx + chdx, \
			b->curChunk->locy + chdy, \
			NULL \
		); /* Will fail if the chunk exists*/ \
		moveBoard(b, chdx, chdy); \
	}

static void drawingHelp(){
	cprintf("----------Drawing mode help------------");
	cprintf("        WASD/Arrow keys to move        ");
	cprintf("          Z - Draw; X - Erase          ");
	SHAREDKEYSPRINT
	cprintf("      ? - Help; ESC/Enter - Normal     ");
	cprintf("---------------------------------------");
}

static inline int getInputDRAW(struct board *b, int c){
	switch(c){
	case 'w': case KEY_UP:    MOVEBUF(-, drawY, RINGMASK,  0, -1); break;
	case 's': case KEY_DOWN:  MOVEBUF(+, drawY,        0,  0,  1); break;
	case 'a': case KEY_LEFT:  MOVEBUF(-, drawX, RINGMASK, -1,  0); break;
	case 'd': case KEY_RIGHT: MOVEBUF(+, drawX,        0,  1,  0); break;
	case 'z': b->curChunk->board[at(drawX, drawY)] = 1; break;
	case 'x': b->curChunk->board[at(drawX, drawY)] = 0; break;
	case '\n': case 27: setInputMode(INM_NORMAL); break;
	case '?': drawingHelp(); break;

	SHAREDKEYS
	}
	return 1;
}

#define INPUTLINE (CHUNKSIZE + 3)

void inputRenderer(void){
	mvaddstr(INPUTLINE, 0, getInputModeName());
	int start = strlen(getInputModeName()) + 1;
	switch(INPUTMODE){
	case INM_TYPE:;
		mvaddstr(INPUTLINE, start, inputBuffer);
		mvprintw(INPUTLINE, start + 1 + inputLength, "%i", inputLength);
	break;
	case INM_DRAW:
		mvprintw(INPUTLINE, start, COORDINATES, drawX, drawY);
		mvaddch(drawY, drawX, '%');
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
		setInputMode(prevMode);
	break;
	default:
		if(inputLength < INPUTMAX){
			inputBuffer[inputLength++] = c;
			inputBuffer[inputLength] = '\0';
		}
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

void setInputMode(enum INM mode){
	switch(mode){
		case INM_NORMAL:
		break;
		case INM_DRAW:
		break;
		case INM_TYPE:
		break;
	}
	INPUTMODE = mode;
}
