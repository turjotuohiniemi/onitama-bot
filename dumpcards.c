#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cards.h"

#define HAS_MOVE(p) (p.x || p.y)

int main(int argc, char *argv[]) {
    for (int i = 0; i < CARDS; ++i) {
        printf("%s:\n", card[i].name);
        char plot[5][5];
        memset(plot, ' ', sizeof plot);
        plot[2][2] = 'x';
        for (int m = 0; m < MAX_MOVES && HAS_MOVE(card[i].movement[m]); ++m) {
            plot[2 + card[i].movement[m].y][2 + card[i].movement[m].x] = '#';
        }
        for (int y = 0; y < 5; ++y) {
            for (int x = 0; x < 5; ++x) {
                putchar(plot[y][x]);
            }
            putchar('\n');
        }
        putchar('\n');
    }
    return EXIT_SUCCESS;
}
