#include <stdbool.h>
#include "highscore.h"
#include "options.h"

#include <efi.h>
#include <efilib.h>

void print_usage(void)
{
    Print(L"usage: 2048 [-cCaAiIrh] [-s SIZE] [-b RATE]\n");
}


/* Initial game options */
struct gameoptions* gameoptions_default(void)
{
    struct gameoptions *opt = AllocatePool(sizeof(struct gameoptions));
    if (!opt) return NULL;

    opt->grid_height = DEFAULT_GRID_HEIGHT;
    opt->grid_width = DEFAULT_GRID_WIDTH;
    opt->spawn_value = DEFAULT_SPAWN_VALUE;
    opt->spawn_rate = DEFAULT_SPAWN_RATE;
    opt->enable_color = DEFAULT_COLOR_FLAG;
    opt->animate = DEFAULT_ANIMATE_FLAG;
    opt->ai = DEFAULT_AI_FLAG;
    opt->interactive = DEFAULT_INTERACTIVE_FLAG;

    return opt;
}

void gameoptions_destroy(struct gameoptions *opt)
{
    FreePool(opt);
}

struct gameoptions* parse_options(struct gameoptions *opt, int argc, CHAR16 **argv)
{
    Print(L"getopt(%d)\n", argc);
#if 0
    int c;
    while ((c = getopt(argc, argv, "aArcCiIhHs:b:")) != -1) {
        switch (c) {
        case 'a':
            opt->animate = true;
            break;
        case 'A':
            opt->animate = false;
            break;
        case 'c':
            opt->enable_color = true;
            break;
        case 'C':
            opt->enable_color = false;
            break;
        case 'i':
            opt->ai = true;
            opt->interactive = false;
            break;
        case 'I':
            opt->ai = true;
            opt->interactive = true;
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
        case 'H':
            printf("%ld\n", highscore_load(NULL));
            exit(0);
        }
    }
#endif

    return opt;
}
