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

/** \file naiv-client.c
 * \brief This file handels the naiv-client "AI" engine.
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
#include "othello.h"
#include "naiv-client.h"

/** \brief Client name
 *
 * The standard name of the "client" 
 */
static char* naivName="Naivus";

/** \brief This function is called from the main Programm.
 *
 * All AI client should have a function with this name, so for now one must
 * only change the make file to have a better AI engine.
 * \return The name of the AI engine.
 */
char* naiv_setAIName(int color) {
    struct timeval t;
    gettimeofday(&t, NULL);
    srandom((unsigned int) t.tv_usec);
    return naivName;
}

/** \brief Make an AI move.
 *
 * This function is a very simple AI engine, which makes a random legal move.
 * There is no AI behind it. For a better AI engine one should use mini-max or
 * alpha-beta.
 * \param p_x The line.
 * \param p_y The column.
 * \param p_board The given board.
 */
void naiv_aiMove(short *p_x, short *p_y, t_board *p_board) {
    int i;
    i =(int) random()%p_board->moveCount;
    *p_x=(int) p_board->moves[2*i]; 
    *p_y=(int) p_board->moves[2*i+1]; 
}
