#ifndef chunk_H
#define chunk_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#define CHUNKSIZE (1 << 4)
#define CHUNKSIZE2 (CHUNKSIZE * CHUNKSIZE)

struct chunk{
	bval board[CHUNKSIZE2];
	int locx, locy;
};

void iterateChunk(struct chunk *chunk);
void calculateChunk(struct chunk *chunk);
bval getv(struct chunk *chunk, int x, int y);
void drawChunk(struct chunk *chunk);

#endif
