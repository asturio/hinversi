/* (c) 2003-2007 Claudio Clemens <asturio at gmx dot net>

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
 
  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
  Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*  
    $Author: claudio $
    $Date: 2008-05-22 10:33:30 $
*/

/** \file matador.c
 * \brief This file handels the matador-client "AI" engine.
 *
 * An alpha-beta AI engine.
 */
#include <config.h>
#ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#endif
#ifdef TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   ifdef HAVE_SYS_TIME_H
#       include <sys/time.h>
#   else
#       include <time.h>
#   endif
#endif
/* For INT_MAX/MIN */
#ifdef HAVE_LIMITS_H
#   include <limits.h>
#endif
#include <stdio.h>
#include "gettext.h"
#define _(string) gettext(string)
#include "othello.h"
#include "matador.h"
#include "ringbuf.h"
#include "macros.h"

/* Weights for board possitions */
static short weights[8][8] = {
    {150, -5,  5,  5,  5,  5, -5,150},
    { -5,-10, -5, -5, -5, -5,-10, -5},
    {  5, -5,  0,  0,  0,  0, -5,  5},
    {  5, -5,  0,  2,  2,  0, -5,  5},
    {  5, -5,  0,  2,  2,  0, -5,  5},
    {  5, -5,  0,  0,  0,  0, -5,  5},
    { -5,-10, -5, -5, -5, -5,-10, -5},
    {150, -5,  5,  5,  5,  5, -5,150}};

/* Move try order */
static short moves2[60][2]={
    // Ecken (100)
    {0,0}, {0,7}, {7,0}, {7,7},
    // Kante nicht neben Ecke (0) /* 50 */
    {0,2}, {0,5}, {2,0}, {2,7}, {5,0}, {5,7}, {7,2}, {7,5},
    // Andere Kanten zu (0) /* 50 */
    {0,3}, {0,4}, {3,0}, {3,7}, {4,0}, {4,7}, {7,3}, {7,4}, 
    // Ecken in 4x4 Feld (1) /* 5 */
    {2,2}, {2,5}, {5,2}, {5,5}, 
    // Kanten zu 4x4 Feld (1) /* 4 */
    {2,3}, {2,4}, {3,2}, {3,5}, {4,2}, {4,5}, {5,3}, {5,4}, 
    // Kanten neben Ecken von 6x6 Feld(2) /* 3 */
    {1,2}, {1,5}, {2,1}, {2,6}, {5,1}, {5,6}, {6,2}, {6,5}, 
    // Andere Kanten im 4x4 Feld, die (2) schliessen /* 2 */
    {1,3}, {1,4}, {3,1}, {3,6}, {4,1}, {4,6}, {6,3}, {6,4},
    // Die böse Kanten neben Ecken /* 50 */
    {0,1}, {0,6}, {1,0}, {1,7}, {6,0}, {6,7}, {7,1}, {7,6}, 
    // Die noch bösere Ecken in 6x6 Feld /* 1 */
    {1,1}, {1,6}, {6,1}, {6,6} 
};

/* internal functions */
static float matadorHeuristic(t_board* p_board);
static int matador_getBoardWeight(t_board *p_board);
static float matador_alphaBeta(t_board* p_board, int max, float alpha, float beta, int depth, short *r, short *c);
static int getSortedMoves(t_board *p_board, short sortedmoves[MAXMOB][2]);

/** \brief Client name
 *
 * The standard name of the "client" 
 */
static const char* matadorName = "ElMatador";

static int turn = 0; /**< My color */

static int st_count = 0; /**< For statistical output */

static int emergency = 0; /**< Flag emergency exit of the alpha-beta-Tree */

static int maxdepth[3] = {MATADOR_MAXDEPTH / 2, 0, MATADOR_MAXDEPTH / 2}; /**< Initialize maxdepth */

/** \brief This function is called from the main Programm.
 *
 * All AI client should have a function with this name, so for now one must
 * only change the make file to have a better AI engine.
 * \return The name of the AI engine.
 */
char* matador_setAIName(int color) {
    struct timeval t;
    gettimeofday(&t, NULL);
    srandom((unsigned int) t.tv_usec);
    maxdepth[0] = max(maxdepth[0], 1);
    maxdepth[2] = max(maxdepth[2], 1);
    return (char*) matadorName;
}

/** \brief Set the depth of the matador-Tree
 * 
 * Calculates a good depth, so the the player wont wait too much 
 * \param oldcount Howmany nodes we visited last time
 * \param count how many nodes we visited this time
 * \param move in what move we are
 */
static void matador_setMaxDepth(int oldcount, int count, int move) {
    float increase = (float)(count - oldcount) / (float) oldcount;
    int  trn = 0;

    trn = turn + 1;

    /* Closing */
    if (move == 46 || move == 47) {
        maxdepth[trn]++;
    }
    if (increase > 3  && count > 850000) {
	maxdepth[trn]--;
    }
    if (increase < -0.4 && count < 350000) {
	maxdepth[trn]++;
    }
    if (count > 1000000) {
	maxdepth[trn]--;
    }
    if (count < 100000) {
	maxdepth[trn]++;
    }

    if (maxdepth[trn] > 60 - move) {
        maxdepth[trn] = 60 - move;
    }
    maxdepth[trn] = min(MATADOR_MAXDEPTH, maxdepth[trn]);
    maxdepth[trn] = max(MATADOR_MINDEPTH, maxdepth[trn]);
}

/** \brief Make a AI move.
 *
 * This function is an alpha-beta AI engine, which makes a the best move using a
 * partial game-tree.
 * \param p_x The line.
 * \param p_y The column.
 * \param p_board The given board.
 */
void matador_aiMove(short *p_x, short *p_y, t_board *p_board) {
    int trn = 0; 
    float m = 0;
    static int oldcount[3] = {0,0,0};
    time_t t_start = 0;
    time_t t_end = 0;
    char buf[254] = "";

    turn = p_board->turn;

    trn = turn + 1;
    
    st_count = 0;
    emergency = 0;

    *p_x = -1;
    *p_y = -1;

    t_start = time(NULL);
    m = matador_alphaBeta(p_board, 1, (float)INT_MIN, (float)INT_MAX, maxdepth[trn], p_x, p_y);
    t_end = time(NULL);
    
    snprintf(buf, sizeof(buf), "(%2.3f/m:[%c%d]/c:%d/d:%d/t:%ld/e:%d)", 
             m, *p_y + 'A', *p_x + 1, st_count, maxdepth[trn], (long) (t_end - t_start), emergency);
    storeMessage(buf);
    if (*p_x == -1) {
	fprintf(stderr, _(" ****** No move selected, strange !!!\n"));
	exit(1);
    }
    matador_setMaxDepth(oldcount[trn], st_count, p_board->move);
    oldcount[trn] = st_count; 
}

/** \brief Compute the heuristic value of the board.
 *
 * This is a little prototype for a heuristic funcion.
 * \param p_board The board
 * \return the heuristic value
 */
static float matadorHeuristic(t_board* p_board) {
    int b = 0, w = 0;  /* we don't need this, but for calling the function */
    /* int mob = 0;*/ 
    int count = 0;
    int weight = 0;
    float result = 0;
    float modify = 0;
    float coef = 0;
    float r = 0.0;

    count = countStones(p_board, &b, &w) * turn ;
    /* Can play till end */
    if (p_board->move + maxdepth[turn + 1] >= 60) {
        return count;
    }

    coef = ((p_board->move + maxdepth[turn + 1]) % 61) / 60.0;
    // mob = getPossibleMoves(p_board) * turn * p_board->turn ; /* more moves for the oponent are bad */ 
    weight = matador_getBoardWeight(p_board) * turn;
    result = count * coef + weight * (3 - coef);

    /* +- 5% */ 
    r = (float) random() / RANDOM_MAX; /* Between 0-1*/
    if (result >= 10 && r <= CHAOSPROB) {
        /* 10% result_prob - 5% result */
        r = (float) random() / RANDOM_MAX;
        modify = r * (result / 10) - (result / 20);
    }

    return result + modify;
}

/** \brief Alpha-Beta game-tree
 *
 * This function search for the best move, using matadorHeuristic and maxdepth.
 * \param p_board The board to be analysed
 * \param max If this is a max level or not. 1 = max, 0 = min
 * \param alpha Current best alpha
 * \param beta Current best beta
 * \param depth How deep we want to search
 * \param r A pointer to the row with the best move for this level. Placeholder.
 * \param c A pointer to the column with the best move for this level. Placeholder.
 * \return The heuristic value of the best move
 */
static float matador_alphaBeta(t_board* p_board, int max, float alpha, float beta, int depth, short *r, short *c){
    t_board tempBoard;
    int i = 0;
    float m = 0.0;

    /* This is not needed at all, we just use it, because we need a parameter
     * for the recursion */

    short tr = 0;
    short tc = 0;
    short sortedMoves[MAXMOB][2]; 

    st_count++;

    if (!emergency && st_count > 1500000) {
        emergency = 1;
    }
    if (emergency) {
	return matadorHeuristic(p_board);
    }
    /* Leaf node */
    if (depth <= 0) {
	return matadorHeuristic(p_board);
    }
   
    /* If we can't move, we should proceed. */
    if ((i = checkPasses(p_board)) > 0) {
	if (i == 2) {
            /* That was a final move, so the score should be 3 times the normal
             * one, because there is no way out */
	    return matadorHeuristic(p_board) * 3.0;
	}
	return matador_alphaBeta(p_board, !max, alpha, beta, depth-2, &tr, &tc) +
	       p_board->turn * turn * 10;
    }
    
    /* Initialize */
    *r = -1;
    *c = -1;
    getSortedMoves(p_board, sortedMoves);
    // memcpy(sortedMoves, p_board->moves, sizeof(short) * 2 * MAXMOB); /* Don't sort now */

    for (i=0; i < p_board->moveCount; i++) {
        if (copyBoard(&tempBoard, p_board)) {
            if (makeMove(sortedMoves[i][0], sortedMoves[i][1], &tempBoard)) {
                getPossibleMoves(&tempBoard); /* Actualise list of possible moves */
                m = matador_alphaBeta(&tempBoard, !max, alpha, beta, depth-1, &tr, &tc);
                if (max) {
                    if (m >= beta) {
                        return beta;
                    }
                    if (m > alpha) {
                        alpha = m;
                        *r = sortedMoves[i][0];
                        *c = sortedMoves[i][1];
                    }
                } else {
                    if (m <= alpha) {
                        return alpha;
                    }
                    if (m < beta) {
                        beta = m;
                        *r = sortedMoves[i][0];
                        *c = sortedMoves[i][1];
                    }
                }
            }
        }
    }
    return (max) ? alpha : beta;
}

/** \brief count the weight of the board.
 * This function is just a heuristic. There is aktually no perfect heuristic or
 * board weight.
 * \param p_board a pointer to the board
 * \return the weight of the board. Negative values are good for Black,
 * positive for white.
 */
static int matador_getBoardWeight(t_board *p_board) {
    int res = 0;
    int x = 0;
    int y = 0;

    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            res += p_board->board[x][y] * weights[x][y];
        }
    }
    return res;
}

/** \brief Sort the moves according to the sort matrix.
 * This function will sort the list of possible moves, so the program will try
 * the best moves first
 * \param p_board a pointer to the board.
 * \param sortedmoves an array to save the moves in sorted order.
 * \return 0 if no error, 1 else.
 */
static int getSortedMoves(t_board *p_board, short sortedmoves[MAXMOB][2]) {
    int i = 0;
    int j = 0;
    int index = 0;
    
    memset(sortedmoves, '\0', sizeof(sortedmoves));
    for (i = 0; i < 60; i++) {
        for (j = 0; j < p_board->moveCount; j++) {
            if ((p_board->moves[j * 2] == moves2[i][0])
                    && (p_board->moves[j * 2 + 1] == moves2[i][1])) {
                sortedmoves[index][0] = p_board->moves[j * 2];
                sortedmoves[index][1] = p_board->moves[j * 2 + 1];
                index++;
                break;
            }
        }
        if (index == p_board->moveCount) {
            break;
        }
    }
    if (index != p_board->moveCount) {
        return 1;
    }
    return 0;
}
