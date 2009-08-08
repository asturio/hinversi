#ifndef _ringbuf_h
#define _ringbuf_h

#define MAXBUFS 10
#define BUFSIZE 256

void nextbuf();
int incIndex(int i);
int getRingIndex();
char *getMessage(int i);
void storeMessage(char *s);

extern char ringbuf[MAXBUFS][BUFSIZE];
extern int ringindex;
#endif
