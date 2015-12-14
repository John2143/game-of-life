#ifndef io_H
#define io_H

#include <stdint.h>
#include <limits.h>

#include <board.h>
#include <chunk.h>

#define PACKEDCHUNKSIZE (CHUNKSIZE2 / CHAR_BIT + 8)

enum ioResult{
	IO_OK,
	IO_FILE,
	IO_HEADER
};

//Write the board pointer to data
//Will be saved in boards/(b->name).golb
//-1 on error, 0 on OK
enum ioResult writeBoard(const struct board *b);
//load the board data into this pointer
//-1 if the board does not exists or is locked, 0 on OK
enum ioResult readBoard(struct board *b);
//Create and read the board into a new board object
//If the board is not found, it creates a new empty board
//Must call free board on this
struct board *readNewBoard(const char *name);

#endif
