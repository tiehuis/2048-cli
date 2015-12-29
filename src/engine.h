#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include "options.h"

#define fatal(msg)\
    do {\
        fprintf(stderr, "line %d: %s\n", __LINE__, msg);\
        abort();\
    } while (0)

struct gamestate {
    /* Game state */
    int *grid_data_ptr;
    int **grid;
    int gridsize;
    int moved;
    long score;
    long score_high;
    long score_last;
    int print_width;
    int blocks_in_play;
    /* Variable command line options */
    struct gameoptions *opts;
};

enum {
    dir_invalid,
    dir_down,
    dir_left,
    dir_right,
    dir_up
};

struct gfx_state;

int gamestate_end_condition(struct gamestate*);
void gamestate_new_block(struct gamestate*);
int  gamestate_tick(struct gfx_state*, struct gamestate*, int, void (*callback)(struct gfx_state*, struct gamestate*));
void gamestate_clear(struct gamestate*);
struct gamestate* gamestate_init(int argc, char **argv);

#endif
