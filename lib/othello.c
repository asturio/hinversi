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

/** \file othello.c 
 * \brief Main Program file.
 *
 * This is the othello file, and it provide the functions needed by all
 * parts, suchs as players, AIs, and a game controler.
 */
#include <config.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#   include <string.h>
#endif
#include "gettext.h"
#define _(string) gettext(string)
#include "othello.h"

static int moveToDirection(int x, int y, int dir, t_board* p_board);
static void changeInDirection(int x, int y, int dir, t_board* p_board);
static void changeBoard(int x, int y, t_board* p_board, int dirs);

/** Pointer for the black name */
static char *blackName = "";
/** Pointer for the white name */
static char *whiteName = "";

/** This is the main board, for game begin.
 * Coordinates are in x,y format.
 * NOTE: x=line, y=column */
static t_board mainBoard = {
    {{0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1,-1, 0, 0, 0},
    {0, 0, 0,-1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}}, 
    BEGINS,
    0,
    {0,0},
    0
};

/** \brief Set the name of a player
 *
 * Set the name of the Black or White Player, calling the right function
 */
void setName(int color, char ccode, char*(*func)(int, char)) {
    if (color == BLACK) {
	blackName = (*func)(color, ccode);
    } else {
	whiteName = (*func)(color, ccode);
    }
}
    
/** \brief Give player name.
 *
 * Give the name of a given player
 * \return Player name or NULL
 */
char* playerName(int color){
    switch(color) {
    case BLACK:
        return blackName;
    case WHITE:
        return whiteName;
    }
    return NULL;
}

/** \brief Make a copy of a board.
 * 
 * This funktions copy one board to another.
 * \param p_newBoard The target board.
 * \param p_board The source board.
 * \return TRUE if the copy is sucessfull
 */
int copyBoard(t_board* p_newBoard, t_board* p_board) {
    if (p_newBoard!=NULL) {
	memset(p_newBoard, '\0', sizeof(t_board));
	memcpy(p_newBoard, p_board, sizeof(t_board));
    } else {
        return FALSE;
    }
    return TRUE;
}

/** \brief Give a clean board
 *
 * Make a new board, with the start positions.
 * \param p_newBoard where to put the board.
 * \return TRUE if generation is sucessfull.
 */
int initBoard(t_board* p_newBoard) {
    int i = copyBoard(p_newBoard, &mainBoard);
    if (i) {
        p_newBoard->move = 1;
        getPossibleMoves(p_newBoard);
    }
    return i;
}

/** \brief Change a board with a move
 * 
 * Change the board with a VALID move x,y. Note that the board will be changed.
 * \param x The line.
 * \param y The column.
 * \param p_board The given board.
 * \param dirs The coded directions.
 */
void changeBoard(int x, int y, t_board* p_board, int dirs) {
    if (dirs & UP) changeInDirection(x, y, UP, p_board);
    if (dirs & DO) changeInDirection(x, y, DO, p_board);
    if (dirs & LE) changeInDirection(x, y, LE, p_board);
    if (dirs & RI) changeInDirection(x, y, RI, p_board);
    if (dirs & UL) changeInDirection(x, y, UL, p_board);
    if (dirs & UR) changeInDirection(x, y, UR, p_board);
    if (dirs & DL) changeInDirection(x, y, DL, p_board);
    if (dirs & DR) changeInDirection(x, y, DR, p_board);
}

/**  \brief Change a given board with a move in only one direction.
 *
 * If a legal move is done, than change the board in a given direction.
 * This function will be called ONLY after isLegalMove, so we can change realy
 * something. 
 * \param x The line coordinate
 * \param y The column coordinate
 * \param dir In which direction the board should be changed
 * \param p_board The board to change
 */
void changeInDirection(int x, int y, int dir, t_board* p_board) {
    int delta_x, delta_y;
    delta_x = delta_y = 0;
    /* First decide the direction-delta */
    switch (dir) {
	case (UP): delta_y=0 ;delta_x=-1 ; break;
	case (DO): delta_y=0 ;delta_x=1 ; break;
	case (LE): delta_y=-1 ;delta_x=0 ; break;
	case (RI): delta_y=1 ;delta_x=0 ; break;
	case (UL): delta_y=-1 ;delta_x=-1 ; break;
	case (UR): delta_y=1 ;delta_x=-1 ; break;
	case (DL): delta_y=-1 ;delta_x=1 ; break;
	case (DR): delta_y=1 ;delta_x=1 ; break;
    }
    /* Change stones */
    do { 
	p_board->board[x][y]=p_board->turn;
	x+=delta_x;
	y+=delta_y;
    } while (p_board->board[x][y] == -(p_board->turn));
}

/** \brief make a move, if it's legal. Pass the turn to the other player. 
 *
 * If a given move is legal, put a stone in that position, change the board and
 * give the turn to the other player.
 * \param x The line coordinate
 * \param y The column coordinate
 * \param p_board The given board
 * \return TRUE if move was possible, FALSE else.
 */
int makeMove(int x, int y, t_board* p_board){
    int dirs;
    if ((dirs=isLegalMove(x, y, p_board))) {
	p_board->board[x][y]=p_board->turn;
	changeBoard(x, y, p_board, dirs);
	p_board->turn *= -1;
	(p_board->move)++;
	return TRUE;
    }
    return FALSE;
}

/** \brief Check if the move is legal. 
 *
 * Check if the place is empty, the coordinates are legal, and some stones can
 * be captured.
 *
 * \param x The line coordinate.
 * \param y The column coordinate.
 * \param p_board The given board
 * \return 0 if is not legal, or a coded combination of directions.
 */
int isLegalMove(int x, int y, t_board* p_board) {
    int dirs=0;
    /* Check if coordinates are legal */
    if (x<0 || y<0 || x>7 || y>7) return dirs;
    /* Check if place is free */
    if (p_board->board[x][y] != FREE) return dirs;
    /* Check if we can make at least a line */
    dirs |= moveToDirection(x, y, UP, p_board);
    dirs |= moveToDirection(x, y, DO, p_board);
    dirs |= moveToDirection(x, y, LE, p_board);
    dirs |= moveToDirection(x, y, RI, p_board);
    dirs |= moveToDirection(x, y, UL, p_board);
    dirs |= moveToDirection(x, y, UR, p_board);
    dirs |= moveToDirection(x, y, DL, p_board);
    dirs |= moveToDirection(x, y, DR, p_board);
    return dirs;
}

/** \brief Check a line in one direction
 *
 * This function is called from isLegalMove(). It just checks if is there a
 * possible line to be made in one direction. isLegalMove call this function 8
 * times for each move.
 * \param x The line coordinate
 * \param y The column coordinate
 * \param dir The given direction
 * \param p_board The given board
 * \return Return the Direction, if there is a possible line in that direction, or 0 else. 
 */
int moveToDirection(int x, int y, int dir, t_board* p_board) {
    int delta_x, delta_y;
    delta_x = delta_y = 0;
    /* First decide the direction-delta */
    switch (dir) {
	case (UP): delta_y=0 ;delta_x=-1 ; break;
	case (DO): delta_y=0 ;delta_x=1 ; break;
	case (LE): delta_y=-1 ;delta_x=0 ; break;
	case (RI): delta_y=1 ;delta_x=0 ; break;
	case (UL): delta_y=-1 ;delta_x=-1 ; break;
	case (UR): delta_y=1 ;delta_x=-1 ; break;
	case (DL): delta_y=-1 ;delta_x=1 ; break;
	case (DR): delta_y=1 ;delta_x=1 ; break;
    }
    /* Check if stone at my side is from opponent */
    x+=delta_x;
    y+=delta_y;
    if (p_board->board[x][y] != -(p_board->turn)) return 0;
    /* Check now if we have a line of the opponent*/
    while (p_board->board[x][y] == -(p_board->turn)) {
	x+=delta_x;
	y+=delta_y;
	if (x<0 || x>7 || y<0 || y>7) return 0;
    }
    /* And now see if the last stone is mine */
    if (p_board->board[x][y] == p_board->turn) return dir;
    return 0;
}

/** \brief Help function get the possible moves AND count them.
 *
 * This function count and saves the possibles moves of the party playing now.
 * \param p_board The board state now.
 * \return the Number of possibles moves
 */
int getPossibleMoves(t_board* p_board) {
    int i,j;
    p_board->moveCount=0;
    for (i=0; i<8; i++) {
	for (j=0; j<8; j++) {
	    if (isLegalMove(i, j, p_board)) {
		if (p_board->moveCount >= MAXMOB) {
		    /* Debug mesg */
		    fprintf(stderr,
                            _("%s: Got %d moves, maximum is %d."
                              " Please increase MAXMOB and"
                              " notify the author.\n"), __FILE__, ++p_board->moveCount, MAXMOB);
		    exit(1);
		}
		p_board->moves[2*p_board->moveCount]=i;
		p_board->moves[2*(p_board->moveCount++)+1]=j;
	    }
	}
    }
    return p_board->moveCount;
}

/** \brief Can we proceed with the game? 
 *
 * Checks if the player must pass, and the follower too.
 * \param p_board The given board
 * \return Number of passes.
 */
int checkPasses(t_board* p_board) {
    if (getPossibleMoves(p_board) == 0) {
	p_board->turn *= -1;
	if (getPossibleMoves(p_board) == 0) {
	    p_board->turn *= -1;
	    return 2; /* 2 passes */
	}
	return 1; /* 1 passes */
    }
    return 0; /* 0 passas */
}

/** \brief Count Stones
 *
 * Count the number of stones in a board.
 * \param p_board The given board
 * \param p_black Number of black stones
 * \param p_white Number of white stones
 * \return The diference of number of stones. A positive value means black is
 * winning, a negative one means white has luck
 */
int countStones(t_board* p_board, int* p_black, int* p_white) {
    int i, j, count = 0;
    *p_black = *p_white = 0;
    for (i=0 ; i<8; i++) 
	for (j=0; j<8; j++){
	    if (p_board->board[i][j] == BLACK) (*p_black)++;
	    if (p_board->board[i][j] == WHITE) (*p_white)++;
	    count += p_board->board[i][j];
	}
    return count;
}

/** \brief Display the disclaimer
 *
 * Display the disclaimer and hints for the GPL
 */
void hinversi_gpl() {
    printf(_("%s Version: %s - (c) Claudio Clemens\n\n"
           "%s comes with ABSOLUTELY NO WARRANTY; for details see the file\n"
           "COPYING.  This is free software, and you are welcome to redistribute it\n"
           "under certain conditions.\n\n"), PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_NAME);
}

/** \brief Convert a string to a board.
 *
 * \param p_board a pointer to the target board
 * \param board a pointer to the source string
 * \param turn tell who will play now
 */
int string2board(t_board *p_board, char *board, int turn) {
    int i = 0;
    int j = 0;
    char c = '\0';
    int move = -3;

    /* Check lenght */
    if (board && *board) {
        if (strlen(board) != 64) {
            return -1; /* Data error */
        }
    } else {
        return -1;
    }
    bzero(p_board, sizeof(t_board));
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            c = board[i * 8 + j];
            if (c == '0') {
                p_board->board[i][j] = FREE;
            } else if (c == '1') {
                p_board->board[i][j] = BLACK;
                move++;
            } else if (c == '2') {
                p_board->board[i][j] = WHITE;
                move++;
            } else {
                return -2; /* Unknown symbol */
            }
        }
    }
    if (turn == BLACK || turn == WHITE) {
        p_board->turn = turn;
    } else {
        return -3; /* Unknown starter */
    }

    p_board->move = move;
    getPossibleMoves(p_board);

    return 0;
}

/** \brief Convert a board to a string
 *
 * \param board a pointer to the target string
 * \param s the size of the target string buffer
 * \param turn a pointer to the turn
 * \param p_board a pointer to the board source
 */
int board2string(char *board, size_t s, int *turn, t_board *p_board) {
    int i = 0;
    int j = 0;
    char *p = NULL;

    if (!p_board || !board || !turn) {
        return -1; /* Data error */
    }
    if (s < 65) {
        return -1; /* Data error */
    }

    bzero(board, s);

    p = board;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            switch (p_board->board[i][j]) {
            case (BLACK):
                *p++ = '1';
                break;
            case (WHITE):
                *p++ = '2';
                break;
            case (FREE):
                *p++ = '0';
                break;
            }
        }
    }

    *turn = p_board->turn;

    return 0;
}
