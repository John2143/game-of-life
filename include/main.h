#ifndef main_H
#define main_H

#include <curses.h>
#include <unistd.h>

#include "global.h"
#include "input.h"
#include "board.h"

//Must be initiated here

//Size of the global status strings, excluding \0
#define GSTRINGSIZE 64

//Set the two string that are displayed below the chunk graph
//TODO: Should this be const char *restrict?
void setgStrings(const char *a, const char *b);

int main(int argc, char **argv);

#endif
