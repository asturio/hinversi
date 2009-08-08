#include <config.h>
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#   include <stdlib.h>
#endif
#include <gettext.h>
#define _(string) gettext(string)
#include <othello.h>

void dumpBoard(t_board *p_board);

int main(int argc, char **argv) {
    t_board board;
    char *arg_board = NULL;
    int arg_turn = 0;
    char ccode = '\0';

    char strboard[65] = "";
    int turn = 0;

    short x = -1;
    short y = -1;


    if (argc < 4) {
        printf(_("Not Enought parameters\n"));
        exit(EXIT_FAILURE);
    }

    arg_board = argv[1];
    arg_turn = atoi(argv[2]);
    ccode = argv[3][0];
    printf(_("board: %s, turn: %d\n"), arg_board, arg_turn);

    string2board(&board, arg_board, arg_turn);

    aiMove(&x, &y, &board, ccode);
    makeMove(x, y, &board);

    board2string(&strboard, sizeof(strboard), &turn, &board);

    printf(_("board: %s, turn: %d choosed: %c%d\n"), strboard, turn, y + 'A', x + 1);

    return 0;
}

void dumpBoard(t_board *p_board) {
    int i = 0;
    int j = 0;
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
    printf("Turn: %d\n", p_board->turn);
    printf("moves: %d\n", p_board->moveCount);
    printf("move: %d\n", p_board->move);
}
