#ifndef _2048_ENGINE
#define _2048_ENGINE

#include <stdio.h>

#define fatal(msg) do { fprintf(stderr, "line %d: %s\n", __LINE__, msg); abort(); } while (0)

typedef enum {
    dir_left  = 'h',
    dir_right = 'l',
    dir_up    = 'k',
    dir_down  = 'j'
} direction;

struct gameoptions {
    size_t grid_height;
    size_t grid_width;
    long goal;
    long spawn_value;
    int spawn_rate;
};

struct gamestate {
    /* Game state */
    long **grid;
    int moved;
    long score;
    long score_high;
    long score_last;
    int print_width;
    /* Options */
    struct gameoptions *opts;
    /* Draw functions */
    void (*ds_draw)(struct gamestate*);
    void (*ds_clear)(void);
};

void gravitate(struct gamestate*, direction, void (*callback)(struct gamestate*));
void merge(struct gamestate*, direction, void (*callback)(struct gamestate*));
int  moves_available(struct gamestate *);
void random_block(struct gamestate *);
int  gamestate_tick(struct gamestate*, direction, void (*callback)(struct gamestate*));
void gamestate_clear(struct gamestate*);;
struct gamestate*   gamestate_init(struct gameoptions *);
struct gameoptions* gameoptions_default(void);
#endif
