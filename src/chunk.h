#ifndef chunk_H
#define chunk_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#define CHUNKSIZE (1 << 4)
#define CHUNKSIZE2 (CHUNKSIZE * CHUNKSIZE)

#define NE_R  0
#define NE_L  1
#define NE_U  2
#define NE_D  3
#define NE_UR 4
#define NE_DR 5
#define NE_UL 6
#define NE_DL 7

struct chunk{
	bval board[CHUNKSIZE2];
	int locx, locy;
	int neighbors[8];
};

void calculateChunk(struct chunk *chunk, bval *change);
void applyChange(struct chunk *chunk, bval *change);
//bval getv(struct chunk *chunk, int x, int y);
void drawChunk(struct chunk *chunk);

#endif
