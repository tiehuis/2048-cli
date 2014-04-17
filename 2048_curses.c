#include <curses.h> 
#include <stdlib.h> /* for malloc  */
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

/* highscore */
int hs;

/* highscore file */
char *file;

/* Merges adjacent squares of the same value together in a certain direction */
void merge(int d)
{
    if (d == DL) {
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
    else if (d == DU) {
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
    else if (d == DR) {
        int i, j;
        for (i = SZ - 1; i >= 0; i--) {
            for (j = SZ - 1; j >= 0; j--) {
                if (j > 0 && g[i][j] && g[i][j] == g[i][j - 1]) {
                    g[i][j] <<= 1;
                    sl += g[i][j];
                    s  += g[i][j];
                    g[i][j-- - 1] = 0;
                }
            }
        }
    }
    else if (d == DD) {
        int i, j;
        for (i = SZ - 1; i >= 0; i--) {
            for (j = SZ - 1; j >= 0; j--) {
                if (j > 0 && g[j][i] && g[j][i] == g[j - 1][i]) {
                    g[j][i] <<= 1;
                    sl += g[j][i];
                    s  += g[j][i];
                    g[j-- - 1][i] = 0;
                }
            }
        }
    }
}

    
/* move all values in the grid to the edge given by the direction pressed */
/* would be nice to generalize this code a little so didn't need four loops */
/* if animations are wanted, then need to alter this so it moves each square one at a time */
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

/* load hiscore */
void load_score() {
    FILE *fd = fopen(file, "r");
    if (fd == NULL) fd = fopen(file, "w+");
    if (fscanf(fd, "%d", &hs) == EOF) hs = 0;
    fclose(fd);
}

/* saves hiscore, but only if playing on standard size grid */
void save_score() {
    if (s > hs && SZ == 4) {
        hs = s;
        FILE *fd = fopen(file, "w+");
        fprintf(fd, "%d", hs);
        fclose(fd);
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

/* places a random block onto the grid - either a 4, or a 2 with a chance of 1:3 respectively */
/* could do this in a much smarter fashion by finding which spaces are free */
void rand_block()
{
    if (space_left()) {
        int x_p, y_p;
        while (g[x_p = rand() % SZ][y_p = rand() % SZ]);
        g[x_p][y_p] = (rand() & 3) ?  2 : 4;
    }
    else {
        endwin();
        printf("\n"
               "YOU LOSE! - Your score was %d\n", s);
        save_score();
        exit(EXIT_SUCCESS);
    }
}

/* quick floor log2(n) */
int flog2(int n)
{
    int k = 0;
    while (n)
        k++, n >>= 1;
    return k;
}

/* draws the grid and fills it with the current values */
/* colors just rotate around, works for now, can be confusing when you have some fairly high values on the board */
void draw_grid(WINDOW *gamewin)
{
    // mvwprintw will sometimes have a useless arg, this is warned, but doesn't affect the program
    char *scr = sl ? "SCORE: %d (+%d)\n" : "SCORE: %d\n";
    mvwprintw(gamewin, 0, 0, scr, s, sl);
    mvwprintw(gamewin, 1, 0, "HISCR: %d\n", hs);

    ITER(SZ*(MAXVAL + 2) + 1, waddch(gamewin, '-')); 
    int i, j, xps = 0, yps = 3;
    for (i = 0; i < SZ; i++, xps = 0, yps++) {
        mvwprintw(gamewin, yps, xps++, "|");
        for (j = 0; j < SZ; j++) {
            if (g[i][j]) {
                wattron(gamewin, COLOR_PAIR(flog2(g[i][j]) & 7));
                mvwprintw(gamewin, yps, xps, "%*d", MAXVAL, g[i][j]);
                wattroff(gamewin, COLOR_PAIR(flog2(g[i][j]) & 7));
                mvwprintw(gamewin, yps, xps + MAXVAL, " |");
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
    /* init ncurses environment */
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    /* init variables */
    file = ".hs2048g";
    hs = 0;
    s  = 0;
    sl = 0;
    SZ = 4;
    MAXVAL = 4;
    CALLOC2D(g, SZ);

    load_score();
    int n_blocks = 1;
    
    /* parse options */
    int c;
    while ((c = getopt(argc, argv, "rchs:b:")) != -1) {
        switch (c) {
            // color support - assumes your terminal can display colours
            // should still work regardless
            case 'c':
                start_color();
                init_pair(1, 1, 0);
                init_pair(2, 2, 0);
                init_pair(3, 3, 0);
                init_pair(4, 4, 0);
                init_pair(5, 5, 0);
                init_pair(6, 6, 0);
                init_pair(7, 7, 0);
                break;
            // different board sizes
            case 's':
                FREE2D(g, SZ);
                int optint = atoi(optarg);
                SZ = optint > 4 ? optint : 4;
                CALLOC2D(g, SZ);
                break;
            // different block spawn rate
            case 'b':
                n_blocks = atoi(optarg);
                break;
            // reset hiscores
            case 'r':
                endwin();
                printf("Are you sure you want to reset your highscores? (Y)es or (N)o\n");
                int response;
                if ((response = getchar()) == 'y' || response == 'Y') {
                    FILE *fd = fopen(file, "w+");
                    fclose(fd);
                }
                exit(EXIT_SUCCESS);
            // help menu
            case 'h':
                endwin();
                printf("Controls:\n"
                       "    hjkl, wasd      Movement\n"
                       "    q               Quit\n"
                       "\n"
                       "Usage:\n"
                       "    2048 [options]\n"
                       "\n"
                       "Options:\n"
                       "    -s <size>       Set the grid border length\n"
                       "    -b <rate>       Set the block spawn rate\n"
                       "    -c              Enables color support\n");
                exit(EXIT_SUCCESS);
        }
    }
    
    int width  = SZ * (MAXVAL + 2) + 1;
    int height = SZ * (MAXVAL + 2) + 3;

    // might center in middle of screen
    WINDOW *gamewin = newwin(height, width, 1, 1);
    keypad(gamewin, TRUE);

    /* random seed */
    srand((unsigned int)time(NULL));
    ITER(2, rand_block());
    draw_grid(gamewin);

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
                save_score();
                exit(EXIT_SUCCESS);
            default:
                goto retry;
        }

        ITER(n_blocks, rand_block());
        draw_grid(gamewin);
    }
}
