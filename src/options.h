#ifndef OPTIONS_H
#define OPTIONS_H

#include <getopt.h>

#define CONSTRAINT_GRID_MIN 4
#define CONSTRAINT_GRID_MAX 20
#define DEFAULT_GRID_HEIGHT 4
#define DEFAULT_GRID_WIDTH 4
#define DEFAULT_GOAL 2048
#define DEFAULT_SPAWN_VALUE 2
#define DEFAULT_SPAWN_RATE 1
#define DEFAULT_COLOR_TOGGLE 0
#define DEFAULT_ANIMATE_TOGGLE 1

struct gameoptions {
    int grid_height;
    int grid_width;
    long goal;
    long spawn_value;
    int spawn_rate;
    int enable_color;
    int animate;
};

void print_usage(void);
struct gameoptions* parse_options(struct gameoptions*, int, char**);
struct gameoptions* gameoptions_default(void);
void gameoptions_destroy(struct gameoptions *opt);

#endif
