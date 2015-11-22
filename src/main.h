#ifndef main_H
#define main_H

#define at(x, y) (y * CHUNKSIZE + x)
#define bval unsigned char

void drawBuf(const bval *inbuf, const char *ascii);
int main(void);

#endif
