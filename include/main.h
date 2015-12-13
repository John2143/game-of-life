#ifndef main_H
#define main_H

#include <curses.h>
#include <unistd.h>

#include "global.h"
#include "input.h"
#include "board.h"
#include "io.h"

//Must be initiated here

//Size of the global status strings, including \0
#define GSTRINGSIZE 64
int CONSIZE; //# of lines that the console can hold

void cprintf(const char *format, ...);
void consoleRenderer(void);

int main(int argc, char **argv);

#endif
