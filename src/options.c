#include <stdio.h>
#include <stdlib.h>
#include "highscore.h"
#include "options.h"

void print_usage(void)
{
    printf("usage: 2048 [-cCaArh] [-s SIZE] [-b RATE]\n");
}


/* Initial game options */
struct gameoptions* gameoptions_default(void)
{
    struct gameoptions *opt = malloc(sizeof(struct gameoptions));
    if (!opt) return NULL;

    opt->grid_height = DEFAULT_GRID_HEIGHT;
    opt->grid_width = DEFAULT_GRID_WIDTH;
    opt->spawn_value = DEFAULT_SPAWN_VALUE;
    opt->spawn_rate = DEFAULT_SPAWN_RATE;
    opt->enable_color = DEFAULT_COLOR_TOGGLE;
    opt->animate = DEFAULT_ANIMATE_TOGGLE;

    return opt;
}

void gameoptions_destroy(struct gameoptions *opt)
{
    free(opt);
}

struct gameoptions* parse_options(struct gameoptions *opt, int argc, char **argv)
{
    int c;
    while ((c = getopt(argc, argv, "aArcCh:s:b:")) != -1) {
        switch (c) {
        case 'a':
            opt->animate = 1;
            break;
        case 'A':
            opt->animate = 0;
            break;
        case 'c':
            opt->enable_color = 1;
            break;
        case 'C':
            opt->enable_color = 0;
            break;
        case 's':;
            /* Stick with square for now */
            int optint = strtol(optarg, NULL, 10);
            if (optint < CONSTRAINT_GRID_MAX && optint > CONSTRAINT_GRID_MIN) {
                opt->grid_height = optint;
                opt->grid_width = optint;
            }
            break;
        case 'b':
            opt->spawn_rate = strtol(optarg, NULL, 10);
            break;
        case 'r':
            highscore_reset();
            exit(0);
        case 'h':
            print_usage();
            exit(0);
        }
    }

    return opt;
}
