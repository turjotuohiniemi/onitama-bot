#ifndef CARD_H_
#define CARD_H_

#define CARDS (16)
#define MAX_MOVES (5)

struct movement {
    int x, y;
};

struct card {
    char *name;
    struct movement movement[MAX_MOVES];
};

extern struct card card[CARDS];

void read_cards(struct card *cardarray[], int count);
int valid_movement(struct movement *m);
int allowed_move(struct card *card, int diffx, int diffy);

#endif
