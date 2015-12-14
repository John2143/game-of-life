#include <io.h>

//TODO fix this only working if enidan is consistant
static void pack32(char *mem, uint32_t n){
	*mem++ = n >> 0x18; *mem++ = n >> 0x10; *mem++ = n >> 0x08; *mem = n;
}

static uint32_t unpack32u(const char *mem){
	return ((uint32_t) mem[0] << 0x18) | ((uint32_t) mem[1] << 0x10) | ((uint32_t) mem[2] << 0x08) | mem[3];
}

static int32_t unpack32i(const char *mem){
	return (int32_t) unpack32u(mem);
}

static void packChunk(const scp c, char *mem){
	int i = CHUNKSIZE2;
	memset(mem, 0, CHUNKSIZE2 / CHAR_BIT);
	int bit = CHAR_BIT;
	while(i){
		*mem |= c->board[--i] << --bit;
		if(!bit){
			mem++;
			bit = CHAR_BIT;
		}
	}
	pack32(mem, c->locx); mem += 4;
	pack32(mem, c->locy); mem += 4;
	/*pack32(mem, c->boardOffset); mem += 4;*/

	/*for(i = 0; i < 8; i++){*/
		/*pack32(mem, c->neighbors[i] ? c->neighbors[i]->boardOffset : -1); mem += 4;*/
	/*}*/
}

static void unpackChunk(scp c, const char *mem){
	int i = CHUNKSIZE2;
	int bit = CHAR_BIT;
	while(i){
		c->board[--i] = (*mem & (1 << --bit)) ? 1 : 0;
		if(!bit){
			mem++;
			bit = CHAR_BIT;
		}
	}
	c->locx = unpack32i(mem); mem += 4;
	c->locy = unpack32i(mem); mem += 4;
	/*c->boardOffset = unpack32i(mem); mem += 4;*/

	/*for(i = 0; i < 8; i++){*/
		/*int boffset = unpack32i(mem); mem += 4;*/
		/*if(boffset >= 0){*/
			/*c->neighbors[i] = c + (boffset - c->boardOffset) * sizeof(struct chunk);*/
		/*}else{*/
			/*c->neighbors[i] = NULL;*/
		/*}*/
	/*}*/
}

#define MEMSIZE max(PACKEDCHUNKSIZE, BOARDNAMELENGTH + 15)
static char mem[MEMSIZE];

enum ioResult writeBoard(const struct board *b){
	sprintf(mem, "boards/%s.golb", b->name);
	FILE *f = fopen(mem, "wb");
	if(!f) return IO_FILE;

	pack32(mem + 0x00, 1);
	pack32(mem + 0x04, b->size);
	pack32(mem + 0x08, b->iterations);

	int chunkDist = 0;
	forEachChunk(b, p){
		if(p == b->curChunk) break;
		chunkDist++;
	}

	pack32(mem + 0x0b, chunkDist);
	fwrite(mem, 1, 0x20, f);

	/*fwrite(b->name, 1, 0x20, f);*/

	forEachChunk(b, p){
		packChunk(p, mem);
		fwrite(mem, 1, sizeof(mem), f);
	}

	fclose(f);
	return IO_OK;
}

enum ioResult readBoard(struct board *b){
	sprintf(mem, "boards/%s.golb", b->name);
	FILE *f = fopen(mem, "rb");
	if(!f) return IO_FILE;

	fread(mem, 1, 0x20, f);
	/*int mapver = unpack32u(mem + 0x0);*/
	b->size       = unpack32u(mem + 0x04);
	b->iterations = unpack32u(mem + 0x08);
	int chunkDist = unpack32u(mem + 0x0b);
	b->first = NULL;

	if(chunkDist > b->size) return IO_HEADER;

	dprintf("Size %i, Iterations %i, curChunk %i\n",
		b->size, b->iterations, chunkDist
	);

	fflush(DEBUG_FILE);

	struct chunk shellChunk;
	for(int i = 0; i < b->size; i++){
		fread(mem, 1, PACKEDCHUNKSIZE, f);
		unpackChunk(&shellChunk, mem);
		dprintf("%i, %i\n", shellChunk.locx, shellChunk.locy);
		for(int y = 0; y < CHUNKSIZE; y++){
			for(int x = 0; x < CHUNKSIZE; x++){
				dprintf("%c", shellChunk.board[at(x, y)] ? '*' : '-');
			}
			dprintf("\n");
		}
		addChunk(b, shellChunk.locx, shellChunk.locy, shellChunk.board);
	}

	b->curChunk = b->first;
	while(chunkDist--) b->curChunk = b->curChunk->next;

	fclose(f);
	return IO_OK;
}

struct board *readNewBoard(const char *name){
	struct board *b = createBoard(name);
	if(readBoard(b) < 0) initializeBoard(b);
	return b;
}
