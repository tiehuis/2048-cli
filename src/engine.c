#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "merge.h"
#include "engine.h"
#include "highscore.h"

/* Utilize block counter to improve some of the functions so they can run
 * quicker */

/* This function will move all blocks in the given game the given direction.
 * The callback function is called after each single move. It can be used to
 * animate the movement of the board. */
static void gravitate(struct gfx_state *s, struct gamestate *g, int d, void (*callback)(struct gfx_state *s, struct gamestate *g))
{

#define swap_if_space(xoff, yoff)\
    do {\
        if (g->grid[x][y] == 0 && g->grid[x+xoff][y+yoff] != 0) {\
            g->grid[x][y] = g->grid[x+xoff][y+yoff];\
            g->grid[x+xoff][y+yoff] = 0;\
            done = 0;\
            g->moved = 1;\
        }\
    } while (0)

    int x, y;
    int done = 0;

    if (d == dir_left) {
        while (!done) {
            done = 1;
            for (x = 0; x < g->opts->grid_width - 1; ++x) {
                for (y = 0; y < g->opts->grid_height; ++y) {
                    swap_if_space(1, 0);
                }
            }
            if (callback)
                callback(s, g);
        }
    }
    else if (d == dir_right) {
        while (!done) {
            done = 1;
            for (x = g->opts->grid_width - 1; x > 0; --x) {
                for (y = 0; y < g->opts->grid_height; ++y) {
                    swap_if_space(-1, 0);
                }
            }
            if (callback)
                callback(s, g);
        }
    }
    else if (d == dir_down) {
        while (!done) {
            done = 1;
            for (y = g->opts->grid_height - 1; y > 0; --y) {
                for (x = 0; x < g->opts->grid_width; ++x) {
                    swap_if_space(0, -1);
                }
            }
            if (callback)
                callback(s, g);
        }
    }
    else if (d == dir_up) {
        while (!done) {
            done = 1;
            for (y = 0; y < g->opts->grid_height - 1; ++y) {
                for (x = 0; x < g->opts->grid_width; ++x) {
                    swap_if_space(0, 1);
                }
            }
            if (callback)
                callback(s, g);
        }
    }
    else {
        fatal("Invalid direction passed to gravitate()");
        /* Not reached */
    }

#undef swap_if_space
}

/* The merge function will combine adjacent blocks with the same value for
 * the given direction. Note, left and right merges will merge in a different
 * order, so they are not identical in all cases.
 *
 * Consider 2 2 2 0
 *
 * Right merging: 4 0 2 0
 * Left merging:  2 0 4 0
 */
static void merge(struct gfx_state *s, struct gamestate *g, int d, void (*callback)(struct gfx_state *s, struct gamestate *g))
{

#define merge_if_equal(xoff, yoff)\
    do {\
        if (g->grid[x][y] && (merge_possible(g->grid[x][y], g->grid[x+xoff][y+yoff]))) {\
            g->grid[x][y] = merge_result(g->grid[x][y], g->grid[x+xoff][y+yoff]);\
            g->grid[x+xoff][y+yoff] = 0;\
            g->blocks_in_play -= 1;\
            g->score_last += merge_value(g->grid[x][y]);\
            g->score += merge_value(g->grid[x][y]);\
            g->moved = 1;\
        }\
    } while (0)

    int x, y;
    g->score_last = 0;

    if (d == dir_left) {
        for (x = 0; x < g->opts->grid_width - 1; ++x) {
            for (y = 0; y < g->opts->grid_height; ++y) {
                merge_if_equal(1, 0);
            }
        }
    }
    else if (d == dir_right) {
        for (x = g->opts->grid_width - 1; x > 0; --x) {
            for (y = 0; y < g->opts->grid_height; ++y) {
                merge_if_equal(-1, 0);
            }
        }
    }
    else if (d == dir_down) {
        for (y = g->opts->grid_height - 1; y > 0; --y) {
            for (x = 0; x < g->opts->grid_width; ++x) {
                merge_if_equal(0, -1);
            }
        }
    }
    else if (d == dir_up) {
        for (y = 0; y < g->opts->grid_height - 1; ++y) {
            for (x = 0; x < g->opts->grid_width; ++x) {
                merge_if_equal(0, 1);
            }
        }
    }
    else {
        fatal("Invalid direction passed to merge()");
        /* Not reached */
    }

    if (callback)
        callback(s, g);

#undef merge_if_equal
}

/* Scan the current board and determine if an end outcome has been reached.
 * -1 indicates a lose condition, 1 indicates a win condition, 0 indicates
 *  end has not yet been reached. */
int gamestate_end_condition(struct gamestate *g)
{
    int ret = -1;
    //size_t blocks_counted = 0;
    int x, y;

    for (x = 0; x < g->opts->grid_width; ++x) {
        for (y = 0; y < g->opts->grid_height; ++y) {
            if (g->grid[x][y] == merge_goal())
                return 1;
            if (!g->grid[x][y] || ((x + 1 < g->opts->grid_width) &&
                        merge_possible(g->grid[x][y], g->grid[x+1][y]))
                    || ((y + 1 < g->opts->grid_height) &&
                        merge_possible(g->grid[x][y], g->grid[x][y+1])))
                ret = 0;
        }
    }

    return ret;
}

/* Place a random block into the current grid */
void gamestate_new_block(struct gamestate *g)
{
    /* Exit early if there are no spaces to place a block */
    if (g->blocks_in_play >= g->gridsize) return;

    int block_number = rand() % (g->gridsize - g->blocks_in_play);

    int x, y, p = 0;
    for (y = 0; y < g->opts->grid_height; ++y) {
        for (x = 0; x < g->opts->grid_width; ++x) {
            if (!g->grid[x][y]) {
                if (p == block_number) {
                    g->grid[x][y] = rand() & 3 ? 1 : 2;
                    g->blocks_in_play += 1;
                    return;
                }
                else {
                    ++p;
                }
            }
        }
    }

    /* This should never be reached; but just in case */
    assert(0);
}

/* This returns the number of digits in the base10 rep of n. The ceiling is
 * taken so this will be one greater than required */
static int digits_ceiling(unsigned int n)
{
    int l = 0;
    while (n) n /= 10, ++l;
    return l + 1;
}

/* Return NULL if we couldn't allocate space for the gamestate. initializating the
 * gamestate will parse the options internally, so any caller should pass argc and argv
 * through this function */
struct gamestate* gamestate_init(int argc, char **argv)
{
    struct gameoptions *opt = gameoptions_default();
    if (argc != 0) parse_options(opt, argc, argv);

    if (!opt) return NULL;

    srand(time(NULL));

    struct gamestate *g = malloc(sizeof(struct gamestate));
    if (!g) goto gamestate_alloc_fail;
    g->gridsize = opt->grid_width * opt->grid_height;

    g->grid_data_ptr = calloc(g->gridsize, sizeof(int));
    if (!g->grid_data_ptr) goto grid_data_alloc_fail;

    g->grid = malloc(opt->grid_height * sizeof(int*));
    if (!g->grid) goto grid_alloc_fail;

    /* Switch to two allocation version */
    int i;
    int **iterator = g->grid;
    for (i = 0; i < g->gridsize; i += opt->grid_width)
        *iterator++ = &g->grid_data_ptr[i];

    g->moved = 0;
    g->score = 0;
    g->score_high = 0;
    g->score_last = 0;
    g->print_width = digits_ceiling(merge_value(merge_goal()));
    g->blocks_in_play = 0;
    g->opts = opt;

    /* Clamp spawn rate to maximum to avoid possible excessive calculation
     * int generation of blocks */
    if (g->opts->spawn_rate > g->gridsize)
        g->opts->spawn_rate = g->gridsize;

    highscore_load(g);

    /* Initial 3 random blocks */
    gamestate_new_block(g);
    gamestate_new_block(g);
    gamestate_new_block(g);
    return g;

grid_alloc_fail:
    free(g->grid_data_ptr);
grid_data_alloc_fail:
    free(g);
gamestate_alloc_fail:
    return NULL;
}

/* A tick is a gravitate, merge then gravitate all in the same direction.
 * the moved variable is set to 0 initially and if the gravitate of merge
 * functions modify it, we can determine which action to take. */
int gamestate_tick(struct gfx_state *s, struct gamestate *g, int d, void (*callback)(struct gfx_state*, struct gamestate*))
{
    /* Reset move. Altered by gravitate and merge if we do move */
    g->moved = 0;
    gravitate(s, g, d, callback);
    merge(s, g, d, callback);
    gravitate(s, g, d, callback);
    return g->moved;
}

/* Free all data associated with the gamestate */
void gamestate_clear(struct gamestate *g)
{
    highscore_save(g);
    gameoptions_destroy(g->opts);
    free(g->grid_data_ptr);   /* Free grid data */
    free(g->grid);      /* Free pointers to data slots */
    free(g);
}
