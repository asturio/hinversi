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
    $Date: 2008-05-22 10:34:02 $
*/

/** \file human-cli.c
 * \brief This file is responsible for interacting with the user.
 *
 * Here are functions for asking the moves of a player, over a cli, readling
 * lines for playernames.
 */
#include <config.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#endif
#include <gettext.h>
#define _(string) gettext(string)
#include <othello.h>
#include "human-cli.h"

static int readline(const int c, char* string);

/** Place for one Player Name. */
static char _name1[31]="";
/** Place for the other Player Name, if there are 2 human players. */
static char _name2[31]="";

/** \brief Display the given Board, and ONLY the board. 
 *  
 *  The board is draw with ascii art.
 *  \param p_board The given board.
 */
void displayBoard(t_board* p_board) {
    int i = 0, j = 0;
    printf("     A   B   C   D   E   F   G   H");
    printf("\n   +---+---+---+---+---+---+---+---+\n ");
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            if (j==0) printf("%d | ", i + 1);
            switch (p_board->board[i][j]) {
                case (BLACK):
                    printf("* | ");
                    break;
                case (WHITE):
                    printf("O | ");
                    break;
                case (FREE):
                    printf("  | ");
                    break;
            }
            if (j==7) printf("%d", i + 1);
        }
        printf("\n   +---+---+---+---+---+---+---+---+\n ");
    }
    printf("    A   B   C   D   E   F   G   H\n");
}

/** \brief Display possible moves
 *
 * Display the possible moves in a given board.
 * \param p_board The board
 */
void displayPossibleMoves(t_board* p_board) {
    int i = 0;
    printf("(%d):", p_board->moveCount);
    for (; i<p_board->moveCount; i++) 
        printf(" %c%d", p_board->moves[i * 2 + 1]+ 'A', p_board->moves[i * 2] + 1);
    printf(". ");
}

/** \brief Display passes.
 *
 * Display the passes.
 * \param p_board The board
 * \param count first(1) or second(2)
 */
void showPass(t_board* p_board, int count) {
    printf(_("%s/%s Can't move, *passing %s*.\n"), 
           playerName(-p_board->turn), (p_board->turn==WHITE)?_("B"):_("W"), (count==1)?_("once"):_("twice")); /* XXX */
    if (count == 1) {
        printf("M: \" , %s/%s -> ", playerName((p_board->turn)), (p_board->turn==WHITE)?_("W"):_("B")); /* XXX */
    } else {
        printf("\n   *** Game Over ***\n\n");
    }
}

/** \brief Display Turn statistics
 *
 * Display the move number, and whose turn it is.
 * \param move Move number
 * \param color What color should play now
 */
void displayTurn(int move, int color) {
    printf("M: %2d, %s/%s -> ", move, playerName(color), (color==BLACK)?_("B"):_("W")); /* XXX*/
}

/** \brief Read a line.
 *
 * Read a line of at most c chars. Note that the string Buffer must be at least
 * c+1, so the ending '\\0' can be appended.
 * \param c Number of chars to read
 * \param string The Buffer, where the read chars are written to
 * \return The count of read chars.
 */
int readline(const int c, char* string) {
    int count = 0;
    int end = 0;
    while (count < c) {
        string[count]=getchar();
        if (string[count] == EOF) exit (1);
        if (string[count] == '\n') {
            end=1;
            break;
        }
        count++;
    }
    string[count]= '\0';
    /* Flush stdin */
    if (!end) while(getchar() !='\n');
    return count;
}

/** \brief Ask for a player move.
 *
 * This function asks for a player move in the given board. It will only accept
 * the move if the move is a legal one.
 * \param p_x Which line.
 * \param p_y Which column.
 * \param p_board The given board.
 * \return The coded directions of the move.
 */
int askMove(short *p_x, short *p_y, t_board *p_board) {
    int dirs = 0;
    char line[3] = "";
    char a = '\0';
    char b = '\0';
    char c = '\0';
    do {
        printf(_("Choose your destiny: "));
        if (readline(2, line) != 2) {
            continue;
        }
        a = line[0]; /* Save number here */
        b = line[1]; /* Save letter here */
        if (a < '1' || a > '8') {
            c = a;
            a = b;
            b = c;
        }
        if (b > 'Z') {
            b -= 32;
        }
        if (a < '1' || a > '8' || b < 'A' || b > 'H') {
            continue;
        }
        *p_x = a - '1';
        *p_y = b - 'A';
    } while (!(dirs=isLegalMove(*p_x, *p_y, p_board)));
    return dirs;
}

/** \brief Read the name of the Player. 
 *
 * Reads the name of the player with a given color (BLACK or WHITE).
 * \param c which color
 * \param ccode bogus parameter
 * \return a pointer to the name.
 */
char *readName(int c, char ccode) {
    char* name = NULL;
    char* color = NULL;
    if (c == BLACK) { /* _name1 not set */
        name = _name1;
        color = _("Black");
    } else { /* We have already ONE player, ask for another */
        name = _name2;
        color = _("White");
    }
    printf(_("Please enter the name %s player: \n"), color);
    readline(30, name);
    return name;
}

/** \brief Display the final result
 *
 * Display the final result of the game.
 * \param p_board The given board
 */
void displayResult(t_board* p_board) {
    int black = 0, white = 0;
    countStones(p_board, &black, &white);
    printf (_("%s (Black): %d x %d :(White) %s\n\n%s Wins!!!\n\n"), 
            playerName(BLACK), black, white, playerName(WHITE), 
            (black > white) ? playerName(BLACK) : 
                              (white > black) ? playerName(WHITE) : _("No one"));
}
