#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "gameplay.h"

int move_counter = 0;

struct gamestate *read_gamestate() {
    struct gamestate *g = malloc(sizeof(*g));
    if (g == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    printf("Blue cards: ");
    read_cards((struct card **) &g->player_card[BLUE][0], PLAYER_CARDS);
    printf("Red cards: ");
    read_cards((struct card **) &g->player_card[RED][0], PLAYER_CARDS);
    printf("Mid-card: ");
    read_cards(&g->swap_card, 1);
    printf("Next in turn (R/B): ");
    int c = getchar();
    if (toupper(c) == 'R')
        g->next_turn = RED;
    else if (toupper(c) == 'B')
        g->next_turn = BLUE;
    else {
        fprintf(stderr, "Unknown color\n");
        exit(EXIT_FAILURE);
    }
    getchar();
    return g;
}

void dump_gamestate(struct gamestate *g) {
    static char *color[PLAYERS] = { "Blue", "Red" };
    for (int i = 0; i < PLAYERS; ++i) {
        printf("%s cards: ", color[i]);
        for (int n = 0; n < PLAYER_CARDS; ++n) {
            printf("%s ", g->player_card[i][n]->name);
        }
        putchar('\n');
    }
    printf("Mid-card: %s\n", g->swap_card->name);
}

int calc_score(struct board *b, int mycolor) {
    int score = 0;
    int opponent = mycolor ^ 1;
    for (int y = 0; y < BOARD_DIM; ++y) {
        for (int x = 0; x < BOARD_DIM; ++x) {
            if ((b->grid[y][x] & PAWN_COLOR_MASK) == mycolor) {
                score += 10;
                if ((y > 0 && (b->grid[y - 1][x] & PAWN_COLOR_MASK) != mycolor)
                 || (x > 0 && (b->grid[y][x - 1] & PAWN_COLOR_MASK) != mycolor)
                 || (x < BOARD_DIM - 1 && (b->grid[y][x + 1] & PAWN_COLOR_MASK) != mycolor)
                 || (y < BOARD_DIM - 1 && (b->grid[y + 1][x] & PAWN_COLOR_MASK) != mycolor))
                {
                    ++score;
                }
            } else if ((b->grid[y][x] & PAWN_COLOR_MASK) == opponent) {
                score -= 5;
            }
        }
    }
    return score;
}

int analyze_game(struct coord *movefrom, struct movement *moveto, char **cardname, int depth,
                 int mycolor, struct gamestate *g, struct board *b)
{
    if (depth > MAX_DEPTH) {
        return calc_score(b, mycolor);
    }
    int best_score = INT_MIN;
    int worst_score = INT_MAX;
    for (int p = 0; p < PAWNS; ++p) {
        if (b->player_pawn[g->next_turn][p].x == CAPTURED_PAWN_IDX)
            continue;
        for (int cnum = 0; cnum < PLAYER_CARDS; ++cnum) {
            for (int m = 0; m < MAX_MOVES; ++m) {
                struct movement *move = &g->player_card[g->next_turn][cnum]->movement[m];
                if (!valid_movement(move))
                    continue;
                ++move_counter;
                struct gamestate newg = *g;
                struct board newb = *b;
                int res = move_pawn(&newb, g->next_turn, p, g->next_turn ? move->x : -move->x,
                                    g->next_turn ? move->y : -move->y);
                if (res == 0) {
                    newg.next_turn ^= 1;
                    newg.swap_card = newg.player_card[g->next_turn][cnum];
                    newg.player_card[g->next_turn][cnum] = g->swap_card;
                    int subscore = analyze_game(NULL, NULL, NULL, depth + 1, mycolor, &newg,
                                                &newb);
                    if (subscore <= worst_score)
                        worst_score = subscore;
                    if (subscore >= best_score)
                        best_score = subscore;
/*DEBUG*/	    if (1 || depth == 0) printf("/&/ %d @ %d/%d/%d '%s' subscore=%d worst=%d best=%d\n", depth, p, cnum, m, g->player_card[g->next_turn][cnum]->name, subscore, worst_score, best_score);
/*DEBUG*/	    print_board(&newb, depth * 6);
                    if (depth == 0 && subscore == best_score) {
#define REGISTER_MOVE \
                        *movefrom = b->player_pawn[g->next_turn][p];		\
                        *moveto = *move; 					\
                        if (!g->next_turn) { 					\
                            moveto->x = -moveto->x; 				\
                            moveto->y = -moveto->y; 				\
                        }							\
                        *cardname = g->player_card[g->next_turn][cnum]->name;
                        REGISTER_MOVE
                    }
                } else if (res > 0) {
                    if (g->next_turn == mycolor) {
/*DEBUG*/           printf("/&/ %d @ %d/%d/%d WIN\n", depth, p, cnum, m);
/*DEBUG*/           print_board(&newb, depth * 6);
                        if (depth == 0) {
                            REGISTER_MOVE
                        }
                        return 999;
                    } else {
/*DEBUG*/               printf("/&/ %d @ %d/%d/%d LOST\n", depth, p, cnum, m);
                        return -999;
                    }
                }
            }
        }
    }
    printf("/&/ <-- %d @ worst=%d best=%d return=%d\n", depth, worst_score, best_score, (depth & 0) ? worst_score : best_score);;
    return (depth & 0) ? worst_score : best_score;
#undef REGISTER_MOVE
}

int register_pawn_move(struct board *b, struct gamestate *g, int fromx, int fromy,
                       int tox, int toy, const char *cardname)
{
    int player;
    int pawn_num;
    if (!find_pawn(b, fromx, fromy, &player, &pawn_num))
        return -1;
    for (int p = 0; p < PLAYERS; ++p) {
        for (int i = 0; i < PLAYER_CARDS; ++i) {
            if (!strcasecmp(g->player_card[p][i]->name, cardname)) {
                int reverse = p ? 1 : -1;
                struct card *tmp = g->player_card[p][i];
                if (!allowed_move(tmp, reverse * (tox - fromx), reverse * (toy - fromy)))
                    return -1;
                g->player_card[p][i] = g->swap_card;
                g->swap_card = tmp;
                return move_pawn(b, player, pawn_num, tox - fromx, toy - fromy);
            }
        }
    }
    fprintf(stderr, "Card '%s' not found\n", cardname);
    return -1;
}
