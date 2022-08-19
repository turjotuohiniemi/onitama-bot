#include <stdio.h>
#include <stdlib.h>

#include "gameplay.h"

static char *players[] = { "blue", "red" };

int main(int argc, char *argv[]) {
    struct board *b;
    b = read_board();
    for (int i = 0; i < PLAYERS; ++i) {
        printf("Score for %s: %d\n", players[i], calc_score(b, i));
    }
    return EXIT_SUCCESS;
}
