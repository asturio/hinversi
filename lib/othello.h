#ifndef _othello_h_
#define _othello_h_

#define WHITE  1	/**< WHITE constant */
#define BLACK -1	/**< BLACK constant */
#define FREE   0	/**< Free place constant */
#define BEGINS BLACK	/**< Which color begins */

#define TRUE  1		/**< Constant for TRUE */
#define FALSE 0		/**< Constant for FALSE */

#define UP 1		/**< Constant for UP */
#define DO 2		/**< Constant for DOWN */
#define LE 4		/**< Constant for LEFT */
#define RI 8		/**< Constant for RIGHT */
#define UL 16		/**< Constant for UP LEFT */
#define UR 32		/**< Constant for UP RIGHT */
#define DL 64		/**< Constant for DOWN LEFT */
#define DR 128		/**< Constant for DOWN RIGHT */

#define MAXMOB 28	/**< Maximum number of possible moves */

#define CHAOSPROB   0.1 /**< Probability of change the heuristic-value */
#define CHAOSNOISE  0.05 /**< Changerate of heuristic-value */

#define RANDOM_MAX 2147483647

/** \brief The Board Structure
 *
 * A nice struct for holding the board and whose turn it is. 
 */
typedef 
struct s_board {
/** \brief The Board.
 *
 * The Board Data
 */
    int board[8][8];
/** \brief Who plays now
 *
 * Which player is going to play next. 
 */
    int turn;
/** \brief The number of elements in moves[]
 *
 * The number of possible moves.
 */
    int moveCount;
/** \brief The array with the moves
 *
 * The possible moves. x=i*2, y=i*2+1
 */
    short moves[MAXMOB*2];

/** \brief Counts the game's moves
 *
 * Which move are we playing now
 */
    int move;
} t_board;

void setName(int color, char ccode, char*(*func)(int, char));
char *playerName(int color);
int copyBoard(t_board *p_newBoard, t_board *p_board);
int initBoard(t_board *p_newBoard);
int makeMove(int x, int y, t_board *p_board);
int isLegalMove(int x, int y, t_board *p_board);
int getPossibleMoves(t_board *p_board);
int checkPasses(t_board *p_board);
int countStones(t_board *p_board, int *p_black, int *p_white);
void hinversi_gpl();
int string2board(t_board *p_board, char *board, int turn);
int board2string(char *board, size_t s, int *turn, t_board *p_board);

#endif
