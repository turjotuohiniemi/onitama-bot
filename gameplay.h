#ifndef GAMEPLAY_H_
#define GAMEPLAY_H_

#include "board.h"

#define MAX_DEPTH (6)

typedef struct card *player_cardptr;

struct gamestate {
    player_cardptr player_card[PLAYERS][PLAYER_CARDS];
    struct card *swap_card;
    enum player_colors next_turn;
};

extern int move_counter;

struct gamestate *read_gamestate();
void dump_gamestate(struct gamestate *g);
int analyze_game(struct coord *movefrom, struct movement *moveto, char **cardname, int depth,
                 int origplayer, struct gamestate *g, struct board *b);
int register_pawn_move(struct board *b, struct gamestate *g, int fromx, int fromy,
                       int tox, int toy, const char *cardname);

#endif
