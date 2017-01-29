#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

struct board standard_board = {
    {
        { BLUE_PAWN, BLUE_PAWN, BLUE_KING, BLUE_PAWN, BLUE_PAWN },
        { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
        { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
        { EMPTY, EMPTY, EMPTY, EMPTY, EMPTY },
        { RED_PAWN, RED_PAWN, RED_KING, RED_PAWN, RED_PAWN }
    },
    {
        { {2, 0}, {0, 0}, {1, 0}, {3, 0}, {4, 0} },
        { {2, 4}, {0, 4}, {1, 4}, {3, 4}, {4, 4} }
    }
};

static int check_board_integrity(struct board *b) {
    static pawn_t pawn_types[PAWNS] = {PAWN_KING_MASK, 0, 0, 0, 0};
    int pawns = 0;
    for (int p = 0; p < PLAYERS; ++p) {
        struct coord *pawn = b->player_pawn[p];
        for (int n = 0; n < PAWNS; ++pawn, ++n) {
            if (pawn->y == CAPTURED_PAWN_IDX && pawn->x == CAPTURED_PAWN_IDX)
                continue;
            if ((b->grid[pawn->y][pawn->x] & PAWN_KING_MASK) != pawn_types[n]) {
                fprintf(stderr, "Board assert failed: pawn %d@%c at [%d,%d] is of wrong kind\n", n, p ? 'R' : 'B', pawn->x, pawn->y);
                exit(EXIT_FAILURE);
            }
            if ((b->grid[pawn->y][pawn->x] & PAWN_COLOR_MASK) != p) {
                fprintf(stderr, "Board assert failed: pawn %d@%c at [%d,%d] is of wrong color\n", n, p ? 'R' : 'B', pawn->x, pawn->y);
                exit(EXIT_FAILURE);
            }
            ++pawns;
        }
    }
    return pawns;
}

void print_board(struct board *b) {
    static int legend[8] = {'b', 'r', ' ', '?', 'B', 'R', '?', '?'};
    static int pawncn[8] = {  1,   1,   0, -99,   1,   1, -99, -99};
    int pawns = 0;
    for (int y = 0; y < BOARD_DIM; ++y) {
        printf("%d  ", y);
        for (int x = 0; x < BOARD_DIM; ++x) {
            putchar(legend[b->grid[y][x]]);
            pawns += pawncn[b->grid[y][x]];
        }
        putchar('\n');
    }
    if (check_board_integrity(b) != pawns) {
        fprintf(stderr, "Board assert failed: internal data structures on pawns do not match\n");
        exit(EXIT_FAILURE);
    }
}

int move_pawn(struct board *b, int player, int pawn_num, int dx, int dy) {
    static int opposite_home_row[PLAYERS] = {BOARD_DIM - 1, 0};
    int newx, newy;
    int winning_move = 0;
    struct coord *p = &b->player_pawn[player][pawn_num];
    newx = p->x + dx;
    if (newx < 0 || newx >= BOARD_DIM)
        return -1;
    newy = p->y + dy;
    if (newy < 0 || newy >= BOARD_DIM)
        return -1;
    if (b->grid[newy][newx] == EMPTY) {
    } else if ((b->grid[newy][newx] & PAWN_COLOR_MASK) == player)
        return -1;
    else {
        for (int i = 0; i < PAWNS; ++i) {
            struct coord *opp = &b->player_pawn[player ^ 1][i];
            if (opp->x == newx && opp->y == newy) {
                winning_move = b->grid[opp->y][opp->x] & PAWN_KING_MASK;
                opp->x = CAPTURED_PAWN_IDX;
                opp->y = CAPTURED_PAWN_IDX;
                break;
            }
        }
    }
    b->grid[newy][newx] = b->grid[p->y][p->x];
    b->grid[p->y][p->x] = EMPTY;
    p->x = newx;
    p->y = newy;
    winning_move |= ((b->grid[newy][newx] & PAWN_KING_MASK) && newx == BOARD_HOME_X
       && newy == opposite_home_row[player]);
    return winning_move;
}

static struct board *alloc_board() {
    struct board *b = malloc(sizeof(*b));
    if (b == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    memset(b, 0, sizeof(*b));
    for (int player = 0; player < PLAYERS; ++player) {
        struct coord *p = b->player_pawn[player];
        for (int pawn = 0; pawn < PAWNS; ++pawn, ++p) {
            p->x = CAPTURED_PAWN_IDX;
            p->y = CAPTURED_PAWN_IDX;
        }
    }
    return b;
}

struct board *read_board() {
    char line[BOARD_DIM + 2];
    int next_pawn[PLAYERS] = { 1, 1 };
    struct board *b = alloc_board();
    printf("Board: (blue's home is on first line)\n");
    for (int i = 0; i < BOARD_DIM; ++i) {
        if (fgets(line, sizeof line, stdin) == NULL)
            exit(EXIT_FAILURE);
        if (line[0] == '\n') {
            memset(line, ' ', sizeof(line) - 2);
            line[sizeof(line) - 2] = '\n';
        }
        if (strlen(line) == 0 || line[strlen(line) - 1] != '\n' || strlen(line) < BOARD_DIM) {
            puts("Malformed line, try again.");
            while (getchar() != '\n') {
            }
            continue;
        }
        char *c = line;
        for (int x = 0; x < BOARD_DIM; ++c, ++x) {
            int val;
            switch (*c) {
            case 'r': val = RED_PAWN; break;
            case 'R': val = RED_KING; break;
            case 'b': val = BLUE_PAWN; break;
            case 'B': val = BLUE_KING; break;
            case ' ': val = EMPTY; break;
            default:
                printf("Bad character '%c', sorry.\n", *c);
                exit(EXIT_FAILURE);
            }
            b->grid[i][x] = val;
            if (val != EMPTY && (val & PAWN_KING_MASK) == 0) {
                struct coord *p = &b->player_pawn[val & PAWN_COLOR_MASK][next_pawn[val & PAWN_COLOR_MASK]];
                p->x = x;
                p->y = i;
                ++next_pawn[val & PAWN_COLOR_MASK];
            } else if (val != EMPTY) {
                struct coord *p = &b->player_pawn[val & PAWN_COLOR_MASK][0];
                p->x = x;
                p->y = i;
            }
        }
    }
    check_board_integrity(b);
    return b;
}

int find_pawn(struct board *b, int x, int y, int *player, int *pawn_num) {
    for (int p = 0; p < PLAYERS; ++p) {
        for (int i = 0; i < PAWNS; ++i) {
            if (b->player_pawn[p][i].x == x && b->player_pawn[p][i].y == y) {
                *player = p;
                *pawn_num = i;
                return 1;
            }
        }
    }
    return 0;
}
