#include <input.h>

//0xAAAAAAVV
//A = args, V = function
//Use IN_RET(func, arg) to encode and IN_GETL AND IN_GETH to decode return val
static int getInput(){
	switch(getch()){
	case 'q':
		return IN_QUIT;
	case 'w':
	case KEY_UP:
		return IN_MOVE_UP;
	case 's':
	case KEY_DOWN:
		return IN_MOVE_DOWN;
	case 'a':
	case KEY_LEFT:
		return IN_MOVE_LEFT;
	case 'd':
	case KEY_RIGHT:
		return IN_MOVE_RIGHT;
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
		setgStrings("Iterating...", "");
		return IN_RET(IN_ITERATE, 1);
	case 'z':
		setgStrings("Iterating 5...", "");
		return IN_RET(IN_ITERATE, 5);
	case 'x':
		setgStrings("Iterating 10...", "");
		return IN_RET(IN_ITERATE, 10);
	case 'c':
		setgStrings("Iterating 100...", "");
		return IN_RET(IN_ITERATE, 100);
	case 'v':
		setgStrings("Iterating 1000...", "");
		return IN_RET(IN_ITERATE, 1000);
	case 'g':
		return IN_RUNGC;
	default:
		setgStrings("Invalid move.", "");
		return IN_NOOP;
	}
}

//TODO allow moving into the void
static int moveBoardHandler(struct board *b, int x, int y){
	if(moveBoard(b, x, y) < 0){
		char buf[GSTRINGSIZE];
		sprintf(buf, "%i, %i is empty.", curChunk(b)->locx + x, curChunk(b)->locy + y);
		setgStrings("You cannot move the board in that direction", buf);
	}
	return IN_NOOP;
}

int input(struct board *b){
	int in = getInput(b);
	char str1[32];
	char str2[32];
	switch(IN_GETL(in)){
	case IN_QUIT: return IN_QUIT;
	case IN_ITERATE:
		in = IN_GETH(in);
		while(in--){
			iterateBoard(b);
		}
	break;
	case IN_MOVE_UP:
		moveBoardHandler(b, 0, -1);
	break;
	case IN_MOVE_DOWN:
		moveBoardHandler(b, 0, 1);
	break;
	case IN_MOVE_LEFT:
		moveBoardHandler(b, -1, 0);
	break;
	case IN_MOVE_RIGHT:
		moveBoardHandler(b, 1, 0);
	break;
	case IN_RUNGC:
		sprintf(str1, "GC run on %i chunks...", b->size);
		int c = collectGarbage(b);
		sprintf(str2, "Removed %i chunks. New size %i.", c, b->size);
		setgStrings(str1, str2);
	break;
	case IN_NOOP:
	break;
	default:
		setgStrings("INTERNAL ERROR", "");
	}
	return 1;
}
