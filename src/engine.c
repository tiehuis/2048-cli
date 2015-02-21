#include <stdlib.h>
#include <time.h>
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
        if (g->grid[x][y] && (g->grid[x][y] == g->grid[x+xoff][y+yoff])) {\
            g->grid[x][y] += g->grid[x+xoff][y+yoff];\
            g->grid[x+xoff][y+yoff] = 0;\
            g->blocks_in_play -= 1;\
            g->score_last += g->grid[x][y];\
            g->score += g->grid[x][y];\
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

    for (int x = 0; x < g->opts->grid_width; ++x) {
        for (int y = 0; y < g->opts->grid_height; ++y) {
            if (g->grid[x][y] >= g->opts->goal)
                return 1;
            if (!g->grid[x][y] || ((x + 1 < g->opts->grid_width) &&
                        (g->grid[x][y] == g->grid[x+1][y]))
                    || ((y + 1 < g->opts->grid_height) &&
                        (g->grid[x][y] == g->grid[x][y+1])))
                ret = 0;
        }
    }

    return ret;
}

/* Place a random block into the current grid */
void gamestate_new_block(struct gamestate *g)
{
    /* pick random square, if it is full, then move forward until we find
     * an empty square. This is biased */

    static int seeded = 0;
    if (!seeded) {
        seeded = 1;
        srand(time(NULL));
    }

    /* Fix up this random number generator */
    /* Method:
     *  -   Find a non-biased index between 0 and blocks_play, n
     *  -   Find the nth 0 element in the array
     *  -   insert a random value there
     */

    /* Error here */
#ifdef SKIP
    size_t block_position = (size_t)rand() % (
            g->opts->grid_width * g->opts->grid_height - g->blocks_in_play);

    size_t i, ps;
    for (i = 0, ps = 0; ps < block_position; ++i) {
        if (!g->grid[i / g->opts->grid_width][i % g->opts->grid_height]) ps++;
    }

    g->grid[i / g->opts->grid_width][i % g->opts->grid_height]
         = (rand() & 3) ? g->opts->spawn_value : g->opts->spawn_value * 2;
#endif

    /* Use rudimentary for now */
    int x, y;
    while (g->grid[x = rand() % g->opts->grid_width][y = rand() % g->opts->grid_height]);
    g->grid[x][y] = (rand() & 3) ? g->opts->spawn_value : g->opts->spawn_value * 2;
    g->blocks_in_play += 1;
}

/* This returns the number of digits in the base10 rep of n. The ceiling is
 * taken so this will be one greater than required */
static int digits_ceiling(unsigned int n)
{
    int l = 0;
    while (n) n /= 10, ++l;
    return l + 1;
}

/* Return NULL if we couldn't allocate space for the gamestate. The opt
 * argument can be passed directly via gameoptions_default i.e
 * *o = gamestate_init(gameoptions_default) is valid, as the delete function
 * will find the pointer to the gameoptions and delete the data accordingly. */
struct gamestate* gamestate_init(struct gameoptions *opt)
{
    if (!opt) return NULL;

    struct gamestate *g = malloc(sizeof(struct gamestate));
    if (!g) goto gamestate_alloc_fail;
    g->gridsize = opt->grid_width * opt->grid_height;

    g->grid_data_ptr = calloc(g->gridsize, sizeof(long));
    if (!g->grid_data_ptr) goto grid_data_alloc_fail;

    g->grid = malloc(opt->grid_height * sizeof(long*));
    if (!g->grid) goto grid_alloc_fail;

    /* Switch to two allocation version */
    long **iterator = g->grid;
    for (int i = 0; i < g->gridsize; i += opt->grid_width)
        *iterator++ = &g->grid_data_ptr[i];

    g->moved = 0;
    g->score = 0;
    g->score_high = 0;
    g->score_last = 0;
    g->print_width = digits_ceiling(opt->goal);
    g->blocks_in_play = 0;
    g->opts = opt;

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
