#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <termios.h>
#include <unistd.h>
#include "2048_engine.h"

#define ITER(x, expr)\
    do {\
        int i;\
        for (i = 0; i < x; ++i) { expr; }\
    } while (0)

#ifdef HAVE_CURSES
void draw_screen(struct gamestate *g)
{
    static WINDOW *gamewin;
    static size_t wh;
    static size_t ww;

    if (!gamewin) {
        wh = g->opts->grid_height * (g->print_width + 2) + 3;
        ww = g->opts->grid_width * (g->print_width + 2) + 1;
        gamewin = newwin(wh, ww, 1, 1);
        keypad(gamewin, TRUE);
    }

    // mvwprintw will sometimes have a useless arg, this is warned, but doesn't affect the program
    char *scr = g->score_last ? "SCORE: %d (+%d)\n" : "SCORE: %d\n";
    mvwprintw(gamewin, 0, 0, scr, g->score, g->score_last);
    mvwprintw(gamewin, 1, 0, "HISCR: %d\n", g->score_high);

    ITER(g->opts->grid_width*(g->print_width + 2) + 1, waddch(gamewin, '-')); 
    int x, y, xps = 0, yps = 3;
    for (y = 0; y < g->opts->grid_height; y++, xps = 0, yps++) {
        mvwprintw(gamewin, yps, xps++, "|");
        for (x = 0; x < g->opts->grid_width; x++) {
            if (g->grid[x][y]) {
                mvwprintw(gamewin, yps, xps, "%*d", g->print_width, g->grid[x][y]);
                mvwprintw(gamewin, yps, xps + g->print_width, " |");
            }
            else {
                ITER(g->print_width + 1, waddch(gamewin, ' '));
                waddch(gamewin, '|');
            }
            xps += (g->print_width + 2);
        }
    }
    ITER(g->opts->grid_height*(g->print_width + 2) + 1, waddch(gamewin, '-')); 
    wrefresh(gamewin);
}

void drawstate_init(void)
{
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    refresh();
}

void drawstate_clear(void)
{
    endwin();
}

int get_keypress(void)
{
    return getch();
}

#elif VT100_COMPATIBLE

#else
struct termios sattr;
void drawstate_clear()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &sattr);
}

void drawstate_init(void)
{
    tcgetattr(STDIN_FILENO, &sattr);

    /* alters terminal stdin to not echo and doesn't need \n before reading getchar */
    struct termios tattr;
    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDOUT_FILENO, TCSANOW, &tattr);
}

void draw_screen(struct gamestate *g)
{
    printf("HISCORE: %ld |", g->score_high);
    printf("| SCORE: %ld ", g->score);
    if (g->score_last) printf("(+%ld)", g->score_last);
    printf("\n");

    // alter this grid_size + 1 to match abitrary grid size
    ITER(g->opts->grid_width, printf("------"));
    printf("-\n");
    int x, y;
    for (y = 0; y < g->opts->grid_height; y++) {
        printf("|");
        for (x = 0; x < g->opts->grid_width; x++) {
            if (g->grid[x][y])
                printf("%*ld |", g->print_width, g->grid[x][y]);
            else
                printf("     |");
        }
        printf("\n");
    }
    ITER(g->opts->grid_width, printf("------"));
    printf("-\n\n");
}

int get_keypress(void)
{
    return fgetc(stdin);
}

#endif

void ddraw(struct gamestate *g)
{
    draw_screen(g);
    usleep(30000);
}

int main(int argc, char **argv)
{
    struct gamestate *g = gamestate_init(gameoptions_default());

    drawstate_init();
    
    while (1) {
        draw_screen(g);

        /* abstract getting keypress */
        int ch;
        do {
            ch = get_keypress();
            if (ch == 'q') { goto endloop; }
        } while (strchr("hjkl", ch) == NULL);

        if (!moves_available(g)) {
            printf("You lose\n");
            break;
        }

        gamestate_tick(g, ch, ddraw);
    }
endloop:

    drawstate_clear();
    gamestate_clear(g);
    return 0;
}
