#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <getopt.h>

#define CONSTRAINT_GRID_MIN 4
#define CONSTRAINT_GRID_MAX 20
#define DEFAULT_GRID_HEIGHT 4
#define DEFAULT_GRID_WIDTH 4
#define DEFAULT_SPAWN_VALUE 2
#define DEFAULT_SPAWN_RATE 1
#define DEFAULT_COLOR_FLAG false
#define DEFAULT_ANIMATE_FLAG true
#define DEFAULT_AI_FLAG false
#define DEFAULT_INTERACTIVE_FLAG true

struct gameoptions {
    int grid_height;
    int grid_width;
    long spawn_value;
    int spawn_rate;
    bool enable_color;
    bool animate;
    bool ai;
    bool interactive;
};

void print_usage(void);
struct gameoptions* parse_options(struct gameoptions*, int, char**);
struct gameoptions* gameoptions_default(void);
void gameoptions_destroy(struct gameoptions *opt);

#endif
