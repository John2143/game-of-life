#ifndef main_H
#define main_H

#ifndef NOCURSES
#include <curses.h>
#endif

#define at(x, y) (y * CHUNKSIZE + x)
#define bval unsigned char

void drawBuf(const bval *inbuf, const char *ascii, int offset);
int main(void);

#endif
