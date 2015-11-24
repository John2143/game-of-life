#ifndef main_H
#define main_H

#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define at(x, y) (y * CHUNKSIZE + x)
#define bval unsigned char
#define colormap unsigned int

//Used to display the playing field. 0 is -, 1 is *
const char *ASCII;
//Used to display the delta field. This is just faster than printf.
const char *SURROUND;

char USECOLOR;

#define COL_SPAWN 1
#define COL_DIE   2
#define COL_DEAD  3
#define COL_LIVE  4

#define GSTRINGSIZE 64

void drawBufColor(const bval *inbuf, const char *ascii, int offset, const colormap *colmap);
void drawBuf(const bval *inbuf, const char *ascii, int offset);
void setgStrings(const char *a, const char *b);

int main(int argc, char **argv);

#endif
