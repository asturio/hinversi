#ifndef _abClient_h_
#define _abClient_h_

#define MAXDEPTH 8

char* ab_setAIName(int color);
void ab_aiMove(short *p_x, short *p_y, t_board *p_board);

#if defined(__STRICT_ANSI__)
extern long int random (void);
extern void srandom (unsigned int seed);
#endif

#endif
