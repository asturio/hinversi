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
    $Date: 2007-03-30 12:31:26 $
*/

/** \file main.c
 * \brief This file controls the game, joining GUI, AI and lib.
 *
 * Main file controling the game. Lets play.
 */
#include <config.h>
#ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#endif
#include <stdio.h>
#include <gettext.h>
#include <getopt.h>
#define _(string) gettext(string)

#include <othello.h>
#include <aiManager.h>
#include <ringbuf.h>
#include "main.h"
#include "human-cli.h"

/** \brief Who is black (default - human)
 *
 * Which player is playing black. 
 */
static char black = 'h';

/** \brief Who is white (default - Client 0)
 *
 * Which player is playing white.
 */
static char white = '1';

/** \brief Options for the CLI
 */
static struct option const long_options[] = {
    {"black", required_argument, 0, 'b'},
    {"white", required_argument, 0, 'w'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {NULL, 0, NULL, 0}
};

/** \brief Function to describe the usage of the program
 */
static usage(int rc) {
    printf(_("%s options:\n"), PACKAGE_NAME);
    printf(_("\t-b\t\tBLACK player [h0123] (default - h)\n\t--black\n"));
    printf(_("\t-w\t\tWHITE player [h0123] (default - 1)\n\t--white\n"));
    printf(_("\t-V\t\tDisplays the version number\n\t--version\n"));
    printf(_("\t-h\t\tThis Help screen\n\t--help\n"));
    exit(rc);
}

/** \brief Function to check if the engine is valid
 */
static int isPossiblePlayer(char c) {
    return (c == '0' || c == '1' || c == '2' || c == '3' || c == 'h');
}

/** \brief Parse the commandline 
 *
 * Parse the command line
 * \param argc like main()
 * \param argv like main()
 */
static int parseCmdLine(int argc, char** argv) {
    int c;

    while ((c = getopt_long(argc, argv, 
                    "b:"	/* Black player */
                    "w:"	/* White player */
                    "h"	        /* help */
                    "V",	/* version */
                    long_options, (int *) 0)) != EOF) {
        switch (c) {
            case 'b':		/* --black */
                black = optarg[0];
                if (!isPossiblePlayer(black)) {
                    printf(_("Engine '%c' not implemented!\n"), black);
                    usage(EXIT_FAILURE);
                }
                break;
            case 'w':		/* --white */
                white = optarg[0];
                if (!isPossiblePlayer(white)) {
                    printf(_("Engine '%c' not implemented!\n"), white);
                    usage(EXIT_FAILURE);
                }
                break;
            case 'V':           /* --Version */
                printf ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
                exit(EXIT_SUCCESS);
            case 'h':           /* --help */
                usage(EXIT_SUCCESS);
            default:
                usage(EXIT_FAILURE);
        }
    }

    return optind;
}

/** \brief Play the game 
 *
 * It's show time baby
 */
int main (int argc, char** argv) {
    short x = 0;
    short y = 0;
    int i = 0;

    int last_msg = 0;
    int msg_count = 0;

#ifdef ENABLE_NLS
#ifdef HAVE_SETLOCALE
    setlocale(LC_ALL, "");
#endif
#endif
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    t_board board;

    parseCmdLine(argc, argv);

    hinversi_gpl();

    if (black == 'h') {
        setName(BLACK, black, (char*(*)(int,char))readName);
    } else {
        setName(BLACK, black, (char*(*)(int,char))setAIName);
    }
    
    if (white == 'h') {
        setName(WHITE, white, (char*(*)(int,char))readName);
    } else {
        setName(WHITE, white, (char*(*)(int,char))setAIName);
    }

    printf(_("This game: %s/B x %s/W.\n"), playerName(BLACK), playerName(WHITE));
    if (!initBoard(&board)) {
        fprintf(stderr, _("ERROR: can't initialize othello board!"));
    }
    for (;board.move<=60;) {
        last_msg = getRingIndex();
	displayBoard(&board);
	displayTurn(board.move, board.turn);
	if ((i = checkPasses(&board)) > 0) {
	    showPass(&board, i);
	    if (i == 2) break;
	}
	displayPossibleMoves(&board);
	if ((board.turn == BLACK && black == 'h') || (board.turn == WHITE && white == 'h')) {
            askMove(&x, &y, &board);
	} else {
            aiMove(&x, &y, &board, (board.turn == BLACK) ? black : white);
            msg_count = getRingIndex();
            printf(" :: ");
            do {
                printf("%s ", getMessage(last_msg));
                last_msg = incIndex(last_msg);
            } while (last_msg != msg_count);
            printf("\n");
        }
	if (!makeMove(x, y, &board))
	    fprintf(stderr , _("%s: Sorry, can't make that move.\n"), __FILE__);
    }
    displayBoard(&board);
    displayResult(&board);
    
    return 0;
}
