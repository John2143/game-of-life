#ifndef global_H
#define global_H

//Used in chunk.hcalculateColorMapFromChange
#define COL_SPAWN 1
#define COL_DIE   2
#define COL_DEAD  3
#define COL_LIVE  4

#define COL_BLUE   1
#define COL_YELLOW 2
#define COL_RED    3
#define COL_WHITE  4
#define COL_GREEN  5
#define COL_CYAN   6

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

#define min(x, y) (((x) > (y)) ? (y) : (x))
#define max(x, y) (((x) < (y)) ? (y) : (x))
#define clamp(x, y, z) (min(z, max(x, y)))

#ifndef NODEBUG
#	include <stdio.h>
#	define DEBUG
	FILE *DEBUG_FILE;
#	define dprintf(...) fprintf(DEBUG_FILE, __VA_ARGS__);
#else
#	define dprintf(...) ;
#endif

#define strncpysafe(dst, src, len) \
	*(dst) = '\0'; \
	strncat(dst, src, len);

typedef unsigned char bval;
typedef unsigned int colormap;

char USECOLOR;
int GCRUNFREQ;

int WINDOWMAX_X;
int WINDOWMAX_Y;

#endif
