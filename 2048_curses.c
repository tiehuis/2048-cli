#include <curses.h> 
#include <stdlib.h> /* for malloc  */
#include <termios.h>/*             */
#include <time.h>   /* for time    */
#include <unistd.h> /* for getopts */

// Repeat an expression y, x times */
#define ITER(x, expr)\
    do {\
        int i;\
        for (i = 0; i < x; i++){ expr;}\
    } while (0)

/* Allocates a square pointer of array of arrays onto heap */
#define CALLOC2D(ptr, sz)\
    do {\
        int i;\
        ptr = calloc(sz, sizeof(*ptr));\
        for (i = 0; i < sz; i++)\
            ptr[i] = calloc(sz, sizeof(*ptr));\
    } while (0)

/* Frees a square pointer of arrays to arrays */
#define FREE2D(ptr, sz)\
    do {\
        int i;\
        for (i = 0; i < sz; i++)\
            free(ptr[i]);\
        free(ptr);\
    } while (0)

/* Define a sequence that should be executed each turn */
#define TURN(x)\
    do {\
        gravitate(x);\
        merge(x);\
        gravitate(x);\
    } while (0)

/* maximum length of any value in grid, for printing */
int MAXVAL;

/* direction enumeration */
enum {DR, DU, DL, DD};

/* grid pointer */
int **g;

/* grid size */
int SZ;

/* score, and last turn score */
int s;
int sl;

/* Merges adjacent squares of the same value together in a certain direction */
void merge(int d)
{
    if (d == DL || d == DR) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ; j++) {
                if (j + 1 < SZ && g[i][j] && g[i][j] == g[i][j + 1]) {
                    g[i][j] <<= 1;
                    sl += g[i][j];
                    s  += g[i][j];
                    g[i][j++ + 1] = 0;
                }
            }
        }
    }
    else {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ; j++) {
                if (j + 1 < SZ && g[j][i] && g[j][i] == g[j + 1][i]) {
                    g[j][i] <<= 1;
                    sl += g[j][i];
                    s  += g[j][i];
                    g[j++ + 1][i] = 0;
                }
            }
        }
    }
}

    
/* move all values in the grid to the edge given by the direction pressed */
/* would be nice to generalize this code a little so didn't need four loops */
void gravitate(int d)
{
    if (d == DL) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ - 1; j++) {
                if (g[i][j]) continue;
                int st = 1;
                while (j + st < SZ && !g[i][j + st]) st++;
                if (j + st < SZ) {
                    g[i][j] = g[i][j + st];
                    g[i][j + st] = 0;
                }
            }
        }
    }
    else if (d == DU) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ - 1; j++) {
                if (g[j][i]) continue;
                int st = 1;
                while (j + st < SZ && !g[j + st][i]) st++;
                if (j + st < SZ) {
                    g[j][i] = g[j + st][i];
                    g[j + st][i] = 0;
                }
            }
        }
    }
    else if (d == DR) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = SZ - 1; j > 0; j--) {
                if (g[i][j]) continue;
                int st = 1;
                while (j - st >= 0 && !g[i][j - st]) st++;
                if (j - st >= 0) {
                    g[i][j] = g[i][j - st];
                    g[i][j - st] = 0;
                }
            }
        }
    }
    else if (d == DD) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = SZ - 1; j > 0; j--) {
                if (g[j][i]) continue;
                int st = 1;
                while (j - st >= 0 && !g[j - st][i]) st++;
                if (j - st >= 0) {
                    g[j][i] = g[j - st][i];
                    g[j - st][i] = 0;
                }
            }
        }
    }
}

/* returns if there are any available spaces left on the grid */
int space_left()
{
    int i, j;
    for (i = 0; i < SZ; i++)
        for (j = 0; j < SZ; j++)
            if (!g[i][j])
                return 1;
    return 0;
}

/* places a random block onto the grid */
/* do this in a smarter fashion */
void rand_block()
{
    if (space_left()) {
        int x_p, y_p;
        while (g[x_p = rand() % SZ][y_p = rand() % SZ]);
        g[x_p][y_p] = 1;
    }
    else {
        endwin();
        printf("\n"
               "YOU LOSE! - Your score was %d\n", s);
        exit(EXIT_SUCCESS);
    }
}

/* draws the grid and fills it with the current values */
void draw_grid(WINDOW *gamewin)
{
    // mvwprintw will sometimes have a useless arg
    char *scr = sl ? "SCORE: %d (+%d)\n" : "SCORE: %d\n";
    mvwprintw(gamewin, 0, 0, scr, s, sl);

    ITER(SZ*(MAXVAL + 2) + 1, waddch(gamewin, '-')); 
    int i, j, xps = 0, yps = 2;
    for (i = 0; i < SZ; i++, xps = 0, yps++) {
        mvwprintw(gamewin, yps, xps++, "|");
        for (j = 0; j < SZ; j++) {
            if (g[i][j]) {
                mvwprintw(gamewin, yps, xps, "%*d |", MAXVAL, g[i][j]);
            }
            else {
                ITER(MAXVAL + 1, waddch(gamewin, ' '));
                waddch(gamewin, '|');
            }
            xps += (MAXVAL + 2);
        }
    }
    ITER(SZ*(MAXVAL + 2) + 1, waddch(gamewin, '-')); 
    wrefresh(gamewin);
}

/* entry point for the program */
/* parses options and stores the main game loop */
int main(int argc, char **argv)
{
    /* init variables */
    s  = 0;
    sl = 0;
    MAXVAL = 4;
    SZ = 4;
    CALLOC2D(g, SZ);

    int n_blocks = 1;
    
    /* parse options */
    int c;
    while ((c = getopt(argc, argv, "hs:b:")) != -1) {
        switch (c) {
            // different board sizes
            case 's':
                FREE2D(g, SZ);
                SZ = atoi(optarg);
                CALLOC2D(g, SZ);
                break;
            // different block spawn rate
            case 'b':
                n_blocks = atoi(optarg);
                break;
            // help menu
            case 'h':
                printf("Controls:\n"
                       "    hjkl, wasd      Movement\n"
                       "    q               Quit\n"
                       "\n"
                       "Usage:\n"
                       "    2048 [options]\n"
                       "\n"
                       "Options:\n"
                       "    -b <size>       Set the grid border length\n"
                       "    -s <rate>       Set the block spawn rate\n");
                exit(EXIT_SUCCESS);
        }
    }
    
    /* init ncurses environment */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    int width  = SZ * (MAXVAL + 2) + 1;
    int height = SZ * (MAXVAL + 2) + 2;

    // might center in middle of screen
    WINDOW *gamewin = newwin(height, width, 1, 1);

    /* random seed */
    srand((unsigned int)time(NULL));
    ITER(2, rand_block());
    draw_grid(gamewin);
    
    // SCREEN IS NOT DRAWING ON THE VERY FIRST ITERATION

    int key;
    while (1) {
        /* will goto this if we didn't get a valid keypress */
        retry:;
        key = wgetch(gamewin);
        sl = 0;

        /* should check if anything changed during merge and if not retry */
        switch (key) {
            case 'h':
            case 'a':
            case KEY_LEFT:
                TURN(DL);
                break;
            case 'l':
            case 'd':
            case KEY_RIGHT:
                TURN(DR);
                break;
            case 'j':
            case 's':
            case KEY_DOWN:
                TURN(DD);
                break;
            case 'k':
            case 'w':
            case KEY_UP:
                TURN(DU);
                break;
            case 'q':
                FREE2D(g, SZ);
                erase();
                refresh();
                endwin();
                exit(EXIT_SUCCESS);
            default:
                goto retry;
        }

        ITER(n_blocks, rand_block());
        draw_grid(gamewin);
    }
}
