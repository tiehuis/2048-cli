#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include "gfx.h"

#define GFX_EXTRA_UP    case KEY_UP:
#define GFX_EXTRA_DOWN  case KEY_DOWN:
#define GFX_EXTRA_RIGHT case KEY_RIGHT:
#define GFX_EXTRA_LEFT  case KEY_LEFT:

#define NUMBER_OF_COLORS 7

#define iterate(n, expression)\
    do {\
        int i;\
        for (i = 0; i < n; ++i) { expression; }\
    } while (0)

struct gfx_state {
    WINDOW *window;
    size_t window_height, window_width;
};

struct gfx_state* gfx_init(struct gamestate *g)
{
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    refresh();

    struct gfx_state *s = malloc(sizeof(struct gfx_state));
    s->window_height = g->opts->grid_height * (g->print_width + 2) + 3;
    s->window_width  = g->opts->grid_width * (g->print_width + 2) + 1;
    s->window = newwin(s->window_height, s->window_width, 1, 1);
    keypad(s->window, TRUE);

    if (g->opts->enable_color && has_colors()) {
        start_color();

        int x = 0;
        init_pair(x++, 1, 0);
        init_pair(x++, 2, 0);
        init_pair(x++, 3, 0);
        init_pair(x++, 4, 0);
        init_pair(x++, 5, 0);
        init_pair(x++, 6, 0);
        init_pair(x++, 7, 0);
        char dummy[x == NUMBER_OF_COLORS ? 1 : -1];
    }

    return s;
}

static int int_log2(int n)
{
    int k = 0;
    while (n)
        ++k, n /= 2;
    return k;
}

void gfx_draw(struct gfx_state *s, struct gamestate *g)
{
    if (g->score_last)
        mvwprintw(s->window, 0, 0, "Score: %d (+%d)\n", g->score, g->score_last);
    else
        mvwprintw(s->window, 0, 0, "Score: %d\n", g->score);

    mvwprintw(s->window, 1, 0, "   Hi: %d\n", g->score_high);
    iterate(g->opts->grid_width * (g->print_width + 2) + 1, waddch(s->window, '-'));

    int x, y,
        xpos = 0,
        ypos = 3;

    for (y = 0; y < g->opts->grid_height; ++y, ++ypos, xpos = 0) {
        mvwprintw(s->window, ypos, xpos++, "|");

        for (x = 0; x < g->opts->grid_width; ++x) {
            if (g->grid[x][y]) {
                wattron(s->window, COLOR_PAIR(int_log2(g->grid[x][y]) % NUMBER_OF_COLORS));
                mvwprintw(s->window, ypos, xpos, "%*d", g->print_width, g->grid[x][y]);
                wattroff(s->window, COLOR_PAIR(int_log2(g->grid[x][y]) % NUMBER_OF_COLORS));
                mvwprintw(s->window, ypos, xpos + g->print_width, " |");
            }
            else {
                iterate(g->print_width + 1, waddch(s->window, ' '));
                waddch(s->window, '|');
            }

            xpos += (g->print_width + 2);
        }
    }

    iterate(g->opts->grid_height * (g->print_width + 2) + 1, waddch(s->window, '-'));
    wrefresh(s->window);
}

int gfx_getch(struct gfx_state *s)
{
    int c = wgetch(s->window);

    /* Flush buffer */
    nodelay(s->window, TRUE);
    while (wgetch(s->window) != ERR);
    nodelay(s->window, FALSE);
    return c;
}

void gfx_sleep(int ms)
{
    usleep(ms * 1000);
}

void gfx_destroy(struct gfx_state *s)
{
    free(s);
    endwin();
}
