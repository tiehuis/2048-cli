#ifndef _2048_ENGINE
#define _2048_ENGINE

#include <stdio.h>

#define CONSTRAINT_GRID_MIN 4
#define CONSTRAINT_GRID_MAX 20
#define DEFAULT_GRID_HEIGHT 4
#define DEFAULT_GRID_WIDTH 4
#define DEFAULT_GOAL 2048
#define DEFAULT_SPAWN_VALUE 2
#define DEFAULT_SPAWN_RATE 1
#define DEFAULT_COLOR_TOGGLE 0
#define DEFAULT_ANIMATE_TOGGLE 1

#define fatal(msg)\
    do {\
        fprintf(stderr, "line %d: %s\n", __LINE__, msg);\
        abort();\
    } while (0)

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
    int enable_color;
    int animate;
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
};

struct gameoptions* parse_options(struct gameoptions*, int, char**);
void gravitate(struct gamestate*, direction, void (*callback)(struct gamestate*));
void merge(struct gamestate*, direction, void (*callback)(struct gamestate*));
int end_condition(struct gamestate *);
void random_block(struct gamestate *);
int  gamestate_tick(struct gamestate*, direction, void (*callback)(struct gamestate*));
void gamestate_clear(struct gamestate*);;
struct gamestate*   gamestate_init(struct gameoptions *);
struct gameoptions* gameoptions_default(void);
#endif
