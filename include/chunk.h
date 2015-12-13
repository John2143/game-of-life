#ifndef chunk_H
#define chunk_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>

#include "global.h"

#define CHUNKSIZE (1 << 4)
#define CHUNKSIZE2 (CHUNKSIZE * CHUNKSIZE)

#define at(x, y) ((y) * CHUNKSIZE + (x))
#define at_BR (CHUNKSIZE2 - 1)
#define at_BL (CHUNKSIZE2 - CHUNKSIZE)
#define at_TR (CHUNKSIZE - 1)
#define at_TL (0)

#define NE_R    0
#define NE_L    1
#define NE_U    2
#define NE_D    3
#define NE_UR   4
#define NE_DR   5
#define NE_UL   6
#define NE_DL   7
#define NE_HERE 8
#define NE_NONE 9

struct chunk{
	bval board[CHUNKSIZE2];
	int locx, locy;
	struct chunk *neighbors[8];
	int boardOffset; //This can be calculated but is just stored here for speed
};

//Calculate an array of bvals that can be used to change the board
void calculateChunk(const struct chunk *chunk, bval *change);
//Use the change array to mutate the chunk using the rules
void applyChange(struct chunk *chunk, const bval *change);
//Draw the current chunk with its changemap
//Running the program with -c will disasble colors
void drawChunk(const struct chunk *chunk);
//returns 0 if it is not empty
//returns 1 if it is empty
int chunkEmpty(const struct chunk *chunk);

//Returns the chunks that will need to be created by the next move
//helper function for generateNewChunks
//returns
//0bXXXX
// U^  ^R
//  D^^L
int newChunks(const struct chunk *c);

static const int neighborOppositeValues[] = {
	NE_L, NE_R, NE_D, NE_U,
	NE_DL, NE_UL, NE_DR, NE_UR,
	NE_HERE, NE_NONE
};

//returns one of the values from the NE enum based on x and y
int neighborDelta(int x, int y);
//returns the opposite of the enum given
#define neighborOpposite(i) (neighborOppositeValues[i])
//reverse of neighborDelta
#define rneighborDelta(x, y) neighborDelta(-(x), -(y))
//#define rneighborDelta(x, y) neighborOpposite(neighborDelta(x, y))

#endif
