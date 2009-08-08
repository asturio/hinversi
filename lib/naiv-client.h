#ifndef _naiv_client_h_
#define _naiv_client_h_

void naiv_aiMove(short *p_x, short *p_y, t_board *p_board);
char *naiv_setAIName(int color);

#if defined(__STRICT_ANSI__)
extern long int random (void);
extern void srandom (unsigned int seed);
#endif

#endif
