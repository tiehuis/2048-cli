#include <stdlib.h>
#include <time.h>
#include "2048_engine.h"

void gravitate(struct gamestate *g, direction d, void (*callback)(struct gamestate *g))
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
                callback(g);
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
                callback(g);
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
                callback(g);
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
                callback(g);
        }
    }
    else {
        fatal("Invalid direction passed to gravitate()");
        /* Not reached */
    }

#undef swap_if_space

}

void merge(struct gamestate *g, direction d, void (*callback)(struct gamestate *g))
{

#define merge_if_equal(xoff, yoff)\
    do {\
        if (g->grid[x][y] && (g->grid[x][y] == g->grid[x+xoff][y+yoff])) {\
            g->grid[x][y] += g->grid[x+xoff][y+yoff];\
            g->grid[x+xoff][y+yoff] = 0;\
            g->score_last = g->grid[x][y];\
            g->score += g->grid[x][y];\
            g->moved = 1;\
        }\
    } while (0)

    int x, y;
        
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
        callback(g);

#undef merge_if_equal
}

int moves_available(struct gamestate *g)
{
    int x, y;
    for (x = 0; x < g->opts->grid_width; ++x) {
        for (y = 0; y < g->opts->grid_height; ++y) {
            if (!g->grid[x][y] || ((x + 1 < g->opts->grid_width) &&
                        (g->grid[x][y] == g->grid[x+1][y]))
                    || ((y + 1 < g->opts->grid_height) && 
                        (g->grid[x][y] == g->grid[x][y+1])))
                return 1;
        }
    }

    return 0;
}

void random_block(struct gamestate *g)
{
    /* pick random square, if it is full, then move forward until we find
     * an empty square. This is biased */

    static int seeded = 0;
    if (!seeded) {
        seeded = 1;
        srand(time(NULL));
    }

    int x = rand() % g->opts->grid_width;
    int y = rand() % g->opts->grid_height;

    while (g->grid[x][y]) {
        x++;
        if (x == g->opts->grid_width) {
            x = 0; y++;
            if (y == g->opts->grid_height)
                y = 0;
        }
    }

    g->grid[x][y] = (rand() & 3) ? g->opts->spawn_value : g->opts->spawn_value * 2;
}

static int flog10(unsigned int n)
{
    int l = 0;
    while (n) n /= 10, ++l;
    return l;
}

struct gamestate* gamestate_init(struct gameoptions *opt)
{
    if (!opt) return NULL;

    struct gamestate *g = malloc(sizeof(struct gamestate));
    if (!g) goto gamestate_alloc_fail;

    //long *grid_back = malloc(opt->grid_width * opt->grid_height * sizeof(long));
    //if (!grid_back) goto grid_back_alloc_fail;
   
    g->grid = malloc(opt->grid_width * sizeof(long*));
    if (!g->grid) goto grid_alloc_fail;

    int i;
    for (i = 0; i < opt->grid_height; ++i)
        g->grid[i] = malloc(opt->grid_height * sizeof(long));

    g->moved = 0;
    g->score = 0;
    g->score_high = 0;
    g->score_last = 0;
    g->print_width = flog10(opt->goal);
    g->opts = opt;

    random_block(g);
    return g;

grid_alloc_fail:
grid_back_alloc_fail:
    free(g);
gamestate_alloc_fail:
    return NULL;
}

struct gameoptions* gameoptions_default(void)
{
    struct gameoptions *opt = malloc(sizeof(struct gameoptions));
    if (!opt) return NULL;

    opt->grid_height = 4;
    opt->grid_width = 4;
    opt->goal = 2048;
    opt->spawn_value = 2;
    opt->spawn_rate = 1;

    return opt;
}

int gamestate_tick(struct gamestate *g, direction d, void (*callback)(struct gamestate*))
{
    g->moved = 0;
    gravitate(g, d, callback);
    merge(g, d, callback);
    gravitate(g, d, callback);
    random_block(g);
    return g->moved;
}

void gamestate_clear(struct gamestate *g)
{
    free(g->opts);
    free(g->grid[0]);
    free(g->grid);
    free(g);
}
