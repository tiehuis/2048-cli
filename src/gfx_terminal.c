#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "merge.h"
#include "gfx.h"

#define iterate(n, expression)\
    do {\
        int i;\
        for (i = 0; i < n; ++i) { expression; }\
    } while (0)

struct gfx_state {
    struct termios oldt, newt;
};

struct gfx_state* gfx_init(struct gamestate *g)
{
    (void) g;

    struct gfx_state *s = malloc(sizeof(struct gfx_state));
    if (!s) return NULL;
    tcgetattr(STDIN_FILENO, &s->oldt);
    s->newt = s->oldt;
    s->newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &s->newt);
    return s;
}

void gfx_draw(struct gfx_state *s, struct gamestate *g)
{
    (void) s;

#ifdef VT100
    printf("\033[2J\033[H");
#endif

    if (g->score_last)
        printf("Score: %ld (+%ld)\n", g->score, g->score_last);
    else
        printf("Score: %ld\n", g->score);

    printf("   Hi: %ld\n", g->score_high);

    iterate((g->print_width + 2) * g->opts->grid_width + 1, printf("-")); printf("\n");

    int x, y;
    for (y = 0; y < g->opts->grid_width; ++y) {
        printf("|");

        for (x = 0; x < g->opts->grid_width; ++x) {
            if (g->grid[x][y])
                printf("%*zd |", g->print_width, merge_value(g->grid[x][y]));
            else
                printf("%*s |", g->print_width, "");
        }
        printf("\n");
    }

    iterate((g->print_width + 2) * g->opts->grid_width + 1, printf("-")); printf("\n\n");
}

int gfx_getch(struct gfx_state *s)
{
    (void) s;
    return getchar();
}

void gfx_sleep(int ms)
{
    usleep(ms * 1000);
}

void gfx_destroy(struct gfx_state *s)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &s->oldt);
    free(s);
}
