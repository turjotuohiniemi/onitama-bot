#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "board.h"
#include "gameplay.h"

extern int vd_counter;

int opponent_move(struct board *b, struct gamestate *g) {
    char cardname[30];
    int fromx, fromy, tox, toy;
    printf("Your move (card nn,nn nn,nn)? ");
    switch (scanf("%20s %d,%d %d,%d", cardname, &fromx, &fromy, &tox, &toy)) {
        case 0:
            exit(EXIT_SUCCESS);
        case 5:
            break;
        default:
            return -1;
    }
    int retval = register_pawn_move(b, g, fromx, fromy, tox, toy, cardname);
    if (retval == -1) {
        puts("Move failed\n");
        return -1;
    }
    if (retval > 0) {
        puts("Ok, you won\n");
        exit(EXIT_SUCCESS);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    struct gamestate *g;
    g = read_gamestate();
    struct board *b;
    b = read_board();
    for (;;) {
        move_counter = 0;
        struct coord movefrom;
        struct movement movediff;
        char *cardname;
        puts("\nThinking...");
        int cost = analyze_game(&movefrom, &movediff, &cardname, 0, g->next_turn, g, b);
        printf("%d moves analyzed\n", move_counter);
        if (cost == 0) {
            puts("Cannot move, skipping turn");
        } else {
            printf("Playing card '%s' on pawn at [%d, %d], move to [%d, %d]\n", cardname,
                   movefrom.x, movefrom.y, movefrom.x + movediff.x, movefrom.y + movediff.y);
            register_pawn_move(b, g, movefrom.x, movefrom.y, movefrom.x + movediff.x,
                               movefrom.y + movediff.y, cardname);
            print_board(b);
            dump_gamestate(g);
        }
        while (opponent_move(b, g) == -1) {
        }
    }
}
