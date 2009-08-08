#ifndef _human_cli_h_
#define _human_cli_h_

void displayBoard(t_board* p_board);
void displayPossibleMoves(t_board* p_board);
void showPass(t_board* p_board, int count);
void displayTurn(int move, int color);
int askMove(short *p_x, short *p_y, t_board *p_board);
char *readName(int color, char ccode);
void displayResult(t_board* p_board);

#endif
