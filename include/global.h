#ifndef global_H
#define global_H

//Used in chunk.hcalculateColorMapFromChange
#define COL_SPAWN 1
#define COL_DIE   2
#define COL_DEAD  3
#define COL_LIVE  4

//X Macro for all the directions
#define EXPAND_DIRS \
	X(-1, -1) \
	X(0, -1) \
	X(1, -1) \
	X(-1, 0) \
	X(1, 0) \
	X(-1, 1) \
	X(0, 1) \
	X(1, 1)

#ifndef NODEBUG
#	define DEBUG
	FILE *DEBUG_FILE;
#	define dprintf(...) fprintf(DEBUG_FILE, __VA_ARGS__);
#else
#	define dprintf(...) ;
#endif

typedef unsigned char bval;
typedef unsigned int colormap;

char USECOLOR;
#endif
