#include <stdbool.h>
#include "highscore.h"
#include "options.h"

#include <efi.h>
#include <efilib.h>

void print_usage(void)
{
    Print(L"usage: 2048 [-c] [-C] [-a] [-A] [-i] [-I] [-r] [-h] [-H] [-s SIZE] [-b RATE]\n");
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
    argc--; argv++;  /* skip program name */
    while (argc > 0) {
	if(argv[0][0] != '-' || argv[0][1] == '\0' || argv[0][2] != '\0') {
	    print_usage();
            Exit(EFI_INVALID_PARAMETER, 0, NULL);
	}
        switch (argv[0][1]) {
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
	    if(argc < 2) {
		print_usage();
		Exit(EFI_INVALID_PARAMETER, 0, NULL);
	    }
            int optint = Atoi(argv[1]);
            if (optint < CONSTRAINT_GRID_MAX && optint > CONSTRAINT_GRID_MIN) {
                opt->grid_height = optint;
                opt->grid_width = optint;
            }
	    argc--; argv++;
            break;
        case 'b':
	    if(argc < 2) {
		print_usage();
		Exit(EFI_INVALID_PARAMETER, 0, NULL);
	    }
            opt->spawn_rate = Atoi(argv[1]);
	    argc--; argv++;
            break;
        case 'r':
            highscore_reset();
            Exit(EFI_SUCCESS, 0, NULL);
        case 'h':
            print_usage();
            Exit(EFI_SUCCESS, 0, NULL);
        case 'H':
            Print(L"%ld\n", highscore_load(NULL));
            Exit(EFI_SUCCESS, 0, NULL);
        }
	argc--; argv++;
    }

    return opt;
}
