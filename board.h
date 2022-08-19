#ifndef BOARD_H_
#define BOARD_H_

#include "cards.h"

#define BOARD_DIM (5)
#define PLAYERS (2)
#define PAWNS (5)
#define PLAYER_CARDS (2)

#define PAWN_COLOR_MASK (3)
#define PAWN_KING_MASK (4)
#define CAPTURED_PAWN_IDX (-9)
#define BOARD_HOME_X (2)

enum pawn {
    BLUE_PAWN	= 0 | 0 | 0,
    RED_PAWN	= 0 | 0 | 1,
    BLUE_KING	= 4 | 0 | 0,
    RED_KING	= 4 | 0 | 1,
    EMPTY	= 0 | 2 | 0
};

enum player_colors {
    BLUE = 0,
    RED = 1
};

typedef enum pawn pawn_t;

struct coord {
    int x;
    int y;
};

struct board {
    pawn_t grid[BOARD_DIM][BOARD_DIM];
    struct coord player_pawn[PLAYERS][PAWNS]; // first one is always the king
};

extern struct board standard_board;

void print_board(struct board *b, int indent);
int move_pawn(struct board *b, int player, int pawn_num, int dx, int dy);
struct board *read_board();
int find_pawn(struct board *b, int x, int y, int *player, int *pawn_num);

#endif
