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

/** \file aiManager.c
 * \brief This file handels all possible "AI" engines.
 *
 * The Manager for the AI engine, calling the right engine, as the player
 * selected it.
 */
#include <config.h>
#ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#endif
#include <stdio.h>
#include "gettext.h"
#define _(string) gettext(string)
#include "othello.h"
#include "aiManager.h"
#include "naiv-client.h"
#include "simpleClient.h"
#include "abClient.h"
#include "matador.h"
#include "ringbuf.h"

/** \brief Which AI playes black
 *
 * Black AI Engine 
 */
int blackAI = 0;

/** \brief Which AI playes white
 *
 * White AI Engine 
 */
int whiteAI = 0;

/** \brief This function is called from the main Programm.
 *
 * This function sets the name of the AI-Engine choosed by the playes
 * \return The name of the AI engine.
 */
char* setAIName(int color, char ccode) {
    switch (ccode) {
    case '0':
        return naiv_setAIName(color);
    case '1':
        return simple_setAIName(color);
    case '2':
        return ab_setAIName(color);
    case '3':
        return matador_setAIName(color);
    default:
        fprintf(stderr, _("Client '%c' not implemented!\n"), ccode);
        exit(2);
    }
}

/** \brief Make a AI move.
 *
 * This function calls the right engine to make the move.
 * \param p_x The line.
 * \param p_y The column.
 * \param p_board The given board.
 * \param ccode The code of the choosen AI-Client (0,1 or 2)
 */
void aiMove(short *p_x, short *p_y, t_board *p_board, char ccode) {
    char buf[254] = "";
    switch (ccode) {
    case '0':
        naiv_aiMove(p_x, p_y, p_board);
        break;
    case '1':
        simple_aiMove(p_x, p_y, p_board);
        break;
    case '2':
        ab_aiMove(p_x, p_y, p_board);
        break;
    case '3':
        matador_aiMove(p_x, p_y, p_board);
        break;
    default:
        fprintf(stderr, _("Client '%c' not implemented!\n"), ccode);
        exit(2);
    }
    snprintf(buf, sizeof(buf), _("Choosed [%c%d].\n"), (*p_y) + 'A', (*p_x) + 1);
    storeMessage(buf);
}
