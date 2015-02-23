#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include "gfx.h"
#include "merge.h"

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

#ifdef INVERT_COLORS
        init_pair(1, COLOR_BLACK, COLOR_RED);
        init_pair(2, COLOR_BLACK, COLOR_GREEN);
        init_pair(3, COLOR_BLACK, COLOR_YELLOW);
        init_pair(4, COLOR_BLACK, COLOR_BLUE);
        init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
        init_pair(6, COLOR_BLACK, COLOR_CYAN);
        init_pair(7, COLOR_BLACK, COLOR_WHITE);
#else
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_BLUE, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_BLACK);
#endif
    }

    return s;
}

void gfx_draw(struct gfx_state *s, struct gamestate *g)
{
    if (g->score_last)
        mvwprintw(s->window, 0, 0, "Score: %d (+%d)\n", g->score, g->score_last);
    else
        mvwprintw(s->window, 0, 0, "Score: %d\n", g->score);

    mvwprintw(s->window, 1, 0, "   Hi: %d\n", g->score_high);

    wattron(s->window, A_DIM);
    iterate(g->opts->grid_width * (g->print_width + 2) + 1, waddch(s->window, '-'));
    wattroff(s->window, A_DIM);

    int x, y,
        xpos = 0,
        ypos = 3;

    for (y = 0; y < g->opts->grid_height; ++y, ++ypos, xpos = 0) {
        wattron(s->window, A_DIM);
        mvwprintw(s->window, ypos, xpos++, "|");
        wattroff(s->window, A_DIM);

        for (x = 0; x < g->opts->grid_width; ++x) {
            if (g->grid[x][y]) {
                wattron(s->window, COLOR_PAIR(g->grid[x][y] % NUMBER_OF_COLORS + 1));
                mvwprintw(s->window, ypos, xpos, "%-*ld", g->print_width, merge_value(g->grid[x][y]));
                mvwprintw(s->window, ypos, xpos + g->print_width, " ");
                wattroff(s->window, COLOR_PAIR(g->grid[x][y] % NUMBER_OF_COLORS + 1));

                wattron(s->window, A_DIM);
                mvwprintw(s->window, ypos, xpos + g->print_width + 1, "|");
                wattroff(s->window, A_DIM);
            }
            else {
                wattron(s->window, A_DIM);
                iterate(g->print_width + 1, waddch(s->window, ' '));
                waddch(s->window, '|');
                wattroff(s->window, A_DIM);
            }

            xpos += (g->print_width + 2);
        }
    }

    wattron(s->window, A_DIM);
    iterate(g->opts->grid_height * (g->print_width + 2) + 1, waddch(s->window, '-'));
    wattroff(s->window, A_DIM);
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
