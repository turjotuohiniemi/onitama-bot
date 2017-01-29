#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cards.h"

#define ZERO {0, 0}

struct card card[CARDS] = {
    { "monkey", { {-1, -1}, {1, -1}, {-1, 1}, {1, 1}, ZERO } },
    { "dragon", { {-2, -1}, {2, -1}, {-1, 1}, {1, 1}, ZERO } },
    { "cobra", { {-1, 0}, {1, -1}, {1, 1}, ZERO, ZERO } },
    { "rabbit", { {-1, 1}, {1, -1}, {2, 0}, ZERO, ZERO } },
    { "mantis", { {-1, -1}, {1, -1}, {0, 1}, ZERO, ZERO } },
    { "eel", { {-1, -1}, {1, 0}, {-1, 1}, ZERO, ZERO } },
    { "goose", { {-1, -1}, {-1, 0}, {1, 0,}, {1, 1}, ZERO } },
    { "crab", { {-2, 0}, {0, -1}, {2, 0}, ZERO, ZERO } },
    { "elephant", { {-1, -1}, {-1, 0}, {1, -1}, {1, 0}, ZERO } },
    { "crane", { {-1, 1}, {0, -1}, {1, 1}, ZERO, ZERO } },
    { "tiger", { {0, -2}, {0, 1}, ZERO, ZERO, ZERO } },
    { "frog", { {-2, 0}, {-1, -1}, {1, 1}, ZERO, ZERO } },
    { "rooster", { {-1, 1}, {-1, 0}, {1, 0}, {1, -1}, ZERO } },
    { "boar", { {-1, 0}, {0, -1}, {1, 0}, ZERO, ZERO } },
    { "ox", { {0, -1}, {1, 0}, {0, 1}, ZERO, ZERO } },
    { "horse", { {-1, 0}, {0, -1}, {0, 1}, ZERO, ZERO } }
};

void read_cards(struct card *cardarray[], int count) {
    char buf[80];
    if (fgets(buf, sizeof buf, stdin) == NULL)
        exit(EXIT_FAILURE);
    if (strlen(buf) == 0 || buf[strlen(buf) - 1] != '\n') {
        fprintf(stderr, "Empty line\n");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    for (char *c = strtok(buf, " \n"); c; c = strtok(NULL, " \n"), ++i) {
        int n;
        for (n = 0; n < CARDS; ++n) {
            if (!strcasecmp(card[n].name, c)) {
                *cardarray++ = &card[n];
                --count;
                break;
            }
        }
        if (n == CARDS) {
            fprintf(stderr, "%s: unknown card\n", c);
            exit(EXIT_FAILURE);
        }
    }
    if (count) {
        fprintf(stderr, "Too few cards\n");
        exit(EXIT_FAILURE);
    }
}

int valid_movement(struct movement *m) {
    return m->x != 0 || m->y != 0;
}

int allowed_move(struct card *card, int diffx, int diffy) {
    struct movement *m = card->movement;
    for (int i = 0; i < MAX_MOVES; ++i, ++m) {
        if (m->x == diffx && m->y == diffy) {
            return 1;
        }
    }
    return 0;
}


