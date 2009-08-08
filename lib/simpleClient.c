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
    $Date: 2007-03-26 18:01:25 $
*/

/** \file simpleClient.c
 * \brief This file handels the simple-client "AI" engine.
 *
 * A very simple AI engine.
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
#include <stdio.h>
#ifdef HAVE_FLOAT_H
#   include <float.h>
#endif
#ifdef HAVE_STRING_H
#   include <string.h>
#endif
#include <stdio.h>
#include "othello.h"
#include "simpleClient.h"
#include "ringbuf.h"

static float simpleHeuristic(t_board* p_board, int turn);

/** \brief Client name
 *
 * The standard name of the "client" 
 */
static char* simpleName="David";

/** \brief This function is called from the main Programm.
 *
 * All AI client should have a function with this name, so for now one must
 * only change the make file to have a better AI engine.
 * \return The name of the AI engine.
 */
/*@exposed@*/ 
char* simple_setAIName() {
    struct timeval t;
    gettimeofday(&t, NULL);
    srandom((unsigned int) t.tv_usec);
    return simpleName;
}

/** \brief Make a AI move.
 *
 * This function is a very simple AI engine, which makes a the best move with one level.
 * For a better AI engine one should use mini-max or alpha-beta. This is just the beginning
 * for a-b.
 * \param p_x The line.
 * \param p_y The column.
 * \param p_board The given board.
 */
void simple_aiMove(short *p_x, short *p_y, t_board *p_board) {
    t_board tempBoard;
    int i;
    int bestI;
    float bestScore, tempScore;
    char buf[254] = "";

    bestI = -1000;
    bestScore = -1000.0;

    memset(&tempBoard, '\0', sizeof(tempBoard));

    for (i=0; i < p_board->moveCount; i++) {
	if (copyBoard(&tempBoard,p_board)) {
	    if (makeMove(p_board->moves[2*i], p_board->moves[2*i+1], &tempBoard)) {
		/* heuristic */
		tempScore = simpleHeuristic(&tempBoard, p_board->turn);
		if (tempScore - bestScore > FLT_EPSILON) {
		    bestScore = tempScore;
		    bestI = i;
		}
	    }
	}
    }
    snprintf(buf, sizeof(buf), "(%.3f/%d) ", bestScore, bestI);
    storeMessage(buf);
    *p_x=(int) p_board->moves[2*bestI]; 
    *p_y=(int) p_board->moves[2*bestI+1];
}

/** \brief Compute the heuristic value of the board.
 *
 * This is a little prototype for a heuristic funcion.
 * \param p_board The board
 * \param turn For who should the analisys be made
 * \return the heuristic value
 */
static float simpleHeuristic(t_board* p_board, int turn) {
    int b, w, mob, count; /* we don't need this, but for calling the function */
    float result;
    float factor = CHAOSNOISE;
    b = w = 0;
    mob = getPossibleMoves(p_board); /* more moves for the oponent are bad */ 
    count = countStones(p_board, &b, &w) * turn; /* numberOfStones */
    result = (float) (count - mob);
    if ((float)random()/RANDOM_MAX <= CHAOSPROB)
	factor=2*CHAOSNOISE*random()/RANDOM_MAX;

    return (float) (result + (factor - CHAOSNOISE) * result);
}
