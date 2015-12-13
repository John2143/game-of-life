#ifndef input_H
#define input_H

#include <curses.h>

#include "global.h"
#include "board.h"
#include "main.h"

//enum IN{
	//IN_QUIT,
	//IN_ITERATE,
	//IN_NOOP,
	//IN_MOVE_LEFT,
	//IN_MOVE_RIGHT,
	//IN_MOVE_DOWN,
	//IN_MOVE_UP,
	//IN_RUNGC,
	//IN_DONE,
	//IN_ZEROCHUNK,
	//IN_READ,
	//IN_WRITE,
	//IN_RENAME,
	//IN_CHANGEMODE
//};

enum INM{
	INM_NORMAL,
	INM_DRAW,
	INM_TYPE
};

enum INM INPUTMODE;

const char *getInputModeName(void);
int input(struct board *b);
void inputRenderer(void);

#endif
