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
    $Date: 2007-03-26 18:01:24 $
*/

/** \file abClient.c
 * \brief This file handels the ab-client "AI" engine.
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
#define _(string) gettext(string) //< gettext macro
#include "othello.h"
#include "abClient.h"
#include "ringbuf.h"
#include "macros.h"

/* Weights for board possitions */
static int weights[8][8] = { { 5, -1, 3, 2, 2, 3, -1,  5},
                             {-1, -2, 1, 1, 1, 1, -2, -1},
                             { 3,  1, 1, 1, 1, 1,  1,  3},
                             { 2,  1, 1, 0, 0, 1,  1,  2},
                             { 2,  1, 1, 0, 0, 1,  1,  2},
                             { 3,  1, 1, 1, 1, 1,  1,  3},
                             {-1, -2, 1, 1, 1, 1, -2, -1},
                             { 5, -1, 3, 2, 2, 3, -1,  5}};

/* internal functions */
static float abHeuristic(t_board *p_board);
static int getBoardWeight(t_board *p_board);
static float alphaBeta(t_board *p_board, float alpha, float beta, int depth, int *selectedMove);

/** \brief Client name
 *
 * The standard name of the "client" 
 */
static const char* abName="Neo";

static int turn = 0; /**< My color */

static int st_count = 0; /**< For statistical output */

static int emergency = 0; /**< Flag emergency exit of the alpha-beta-Tree */

static float factor = 1; /**< How far are we in the game */

static int maxdepth[3] = {MAXDEPTH / 2, 0, MAXDEPTH / 2}; /**< Initialize maxdepth */

/** \brief This function is called from the main Programm.
 *
 * All AI client should have a function with this name, so for now one must
 * only change the make file to have a better AI engine.
 * \return The name of the AI engine.
 */
char* ab_setAIName(int color) {
    struct timeval t;
    gettimeofday(&t, NULL);
    srandom((unsigned int) t.tv_usec);
    maxdepth[0] = max(maxdepth[0], 1);
    maxdepth[2] = max(maxdepth[2], 1);
    return (char*) abName;
}

/** \brief Set the depth of the ab-Tree
 * 
 * Calculates a good depth, so the the player wont wait too much 
 * \param oldcount Howmany nodes we visited last time
 * \param count how many nodes we visited this time
 * \param move in what move we are
 */
static void ab_setMaxDepth(int oldcount,int count,int move) {
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
}

/** \brief Make a AI move.
 *
 * This function is an alpha-beta AI engine, which makes a the best move using a
 * partial game-tree.
 * \param p_x The line.
 * \param p_y The column.
 * \param p_board The given board.
 */
void ab_aiMove(short *p_x, short *p_y, t_board *p_board) {
    int bestMove = 0;
    int trn = 0; 
    float m = 0;
    static int oldcount[3] = {0,0,0};
    time_t t_start = 0;
    time_t t_end = 0;
    char buf[254] = "";

    turn = p_board->turn;

    trn = turn + 1;
    
    factor = (p_board->move)/30.0;
   
    st_count = 0;
    emergency = 0;

    t_start = time(NULL);
    m = alphaBeta(p_board, (float)INT_MIN, (float)INT_MAX, maxdepth[trn], &bestMove);
    t_end = time(NULL);
    
    snprintf(buf, sizeof(buf), "(%2.3f/m:%d/c:%d/d:%d/t:%ld/e:%d)", 
             m, bestMove, st_count, maxdepth[trn], (long) (t_end - t_start), emergency);
    storeMessage(buf);
    if (bestMove == -1) {
	fprintf(stderr, _(" ****** No move selected, strange !!!\n"));
	exit(1);
    }
    *p_x=(int) p_board->moves[2*bestMove]; 
    *p_y=(int) p_board->moves[2*bestMove+1];
    ab_setMaxDepth(oldcount[trn], st_count, p_board->move);
    oldcount[trn] = st_count; 
}

/** \brief Compute the heuristic value of the board.
 *
 * This is a little prototype for a heuristic funcion.
 * \param p_board The board
 * \return the heuristic value
 */
static float abHeuristic(t_board* p_board) {
    int b = 0, w = 0;  /* we don't need this, but for calling the function */
    int mob = 0;
    int count = 0;
    int weight = 0;
    float result = 0;
    float modify = CHAOSNOISE;

    count = countStones(p_board, &b, &w) * turn; /* numberOfStones */
    mob = getPossibleMoves(p_board) * turn * p_board->turn ; /* more moves for the oponent are bad */ 
    weight = getBoardWeight(p_board) * turn;
    result = factor * count +  2 * mob + weight;
    if ((float)random()/RANDOM_MAX <= CHAOSPROB) {
	modify = 2 * CHAOSNOISE * random() / RANDOM_MAX;
    }

    return result + (modify - CHAOSNOISE) * result;
}

/** \brief Alpha-Beta game-tree
 *
 * This function search for the best move, using abHeuristic and maxdepth.
 * \param p_board The board to be analysed
 * \param alpha Current best alpha
 * \param beta Current best beta
 * \param depth How deep we want to search
 * \param selectedMove The selected move of for this level. Placeholder. It is
 * only important at the first level.
 * \return The heuristic value of the best move
 */
static float alphaBeta(t_board* p_board, float alpha, float beta, int depth, int* selectedMove){
    t_board tempBoard;
    int i = 0;
    float m = 0.0;

    /* This is not needed at all, we just use it, because we need a parameter
     * for the recursion */

    int childSelectedMove = 0; 

    st_count++;

    if (!emergency && st_count > 1500000) {
        emergency = 1;
    }
    if (emergency) {
	return abHeuristic(p_board);
    }
    /* Leaf node */
    if (depth <= 0) {
	return abHeuristic(p_board);
    }
    
    /* If we can't move, we should proceed. */
    if ((i = checkPasses(p_board)) > 0) {
	if (i == 2) {
            /* That was a final move, so the score should be 3 times the normal
             * one, because there is no way out */
	    return abHeuristic(p_board) * 3.0;
	}
	return alphaBeta(p_board, alpha, beta, depth-2, &childSelectedMove) +
	       p_board->turn * turn * 10;
    }
    
    *selectedMove = -1;
    if (p_board->turn == turn) { /* Max level */
	for (i=0; i < p_board->moveCount; i++) {
	    if (copyBoard( &tempBoard, p_board)) {
		if (makeMove(p_board->moves[2*i], p_board->moves[2*i+1], &tempBoard)) {
		    getPossibleMoves(&tempBoard); /* Actualise list of possible moves */
		}
	    }
	    m = alphaBeta( &tempBoard, alpha, beta, depth-1, &childSelectedMove);
	    if (m > alpha) {	
		alpha = m;
		*selectedMove = i;
	    }
	    if (alpha >= beta) {
		return alpha;
	    }
	}
	return alpha;
    } else { /* Min Level */
	for (i=0; i < p_board->moveCount; i++) {
	    if (copyBoard( &tempBoard, p_board)) {
		if (makeMove(p_board->moves[2*i], p_board->moves[2*i+1], &tempBoard)) {
		    getPossibleMoves(&tempBoard);
		}
	    }
	    m = alphaBeta( &tempBoard, alpha, beta, depth-1, &childSelectedMove);
	    if (m < beta) {
		beta = m;
		*selectedMove = i;
	    }
	    if (alpha >= beta) {
		return beta;
	    }
	}
	return beta;
    }
    fprintf(stderr, _("You shouldn't be seeing this message %s, %d.\n"), __FILE__, __LINE__);
    return -1;
}

/** \brief count the weight of the board.
 * This function is just a heuristic. There is aktually no perfect heuristic or
 * board weight.
 * \param p_board a pointer to the board
 * \return the weight of the board. Negative values are good for Black,
 * positive for white.
 */
static int getBoardWeight(t_board *p_board) {
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
