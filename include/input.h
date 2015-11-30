#ifndef input_H
#define input_H

#include <curses.h>

#include "board.h"
#include "main.h"

#define IN_MASK_BITS 8
#define IN_MASK ((1 << IN_MASK_BITS) - 1)

#define IN_RET(func, arg) ((func) | ((arg) << IN_MASK_BITS))
#define IN_GETL(ret) (ret & IN_MASK)
#define IN_GETH(ret) (ret >> IN_MASK_BITS)

#define IN_QUIT       0
#define IN_ITERATE    1
#define IN_NOOP       2
#define IN_MOVE_LEFT  3
#define IN_MOVE_RIGHT 4
#define IN_MOVE_DOWN  5
#define IN_MOVE_UP    6
#define IN_RUNGC      7
#define IN_DONE       8
#define IN_ZEROCHUNK  9

int input(struct board *b);

#endif
