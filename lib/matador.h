#ifndef _matador_h_
#define _matador_h_

#define MATADOR_MAXDEPTH 8
#define MATADOR_MINDEPTH 3

char *matador_setAIName(int color);
void matador_aiMove(short *p_x, short *p_y, t_board *p_board);

#if defined(__STRICT_ANSI__)
extern long int random (void);
extern void srandom (unsigned int seed);
#endif

#endif
