#ifndef _simpleClient_h_
#define _simpleClient_h_

char *simple_setAIName();
void simple_aiMove(short *p_x, short *p_y, t_board *p_board);

#if defined(__STRICT_ANSI__)
extern long int random (void);
extern void srandom (unsigned int seed);
#endif

#endif
