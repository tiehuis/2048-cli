/*
 * 2048.c - The game 2048 for your Linux terminal.
 *
 * Copyright (c) 2014 Tiehuis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Things to do:
 *  - Finish cleaning up code.
 *  - Ask user if they would like to play again instead of exiting immediately.
 *  - Add an actual goal (what happens when we reach 2048?).
 *  - Keep track list of past scores, and store in a better fashion independent
 *    of cwd. Also, store not in plaintext.
 * */

#include <stdlib.h>    /* for malloc           */
#include <stdio.h>     /* for printf, FILE     */
#include <string.h>    /* for strlen, strrchr  */
#include <sys/stat.h>  /* for mkdir            */
#include <time.h>      /* for time             */
#include <unistd.h>    /* for getopts          */

#ifndef NO_CURSES
  #include <ncurses.h> /* for ncurses terminal */
#else  /* ifndef NO_CURSES */
  #include <termios.h> /* for plain terminal   */
#endif /* ifndef NO_CURSES */


/*     Constants     */
#define DATADIR_NAME "2048"
#define HIGHSCORE_FILE_NAME "highscore"
#define DEFAULT_GRID_SIZE 4
#define HISCORE_FILE get_highscore_file()
#define USAGE_STR\
   "Usage:\n"\
   "    ./2048 [options]\n"\
   "\n"\
   "Controls:\n"\
   "    hjkl, wasd      Movement\n"\
   "    q               Quit\n"\
   "\n"\
   "Options:\n"\
   "    -s <size>       Set the grid border length\n"\
   "    -b <rate>       Set the block spawn rate\n"\
   "    -c              Enables color support (ncurses version only)\n"\
   "    -C              Disables color support (ncurses version only)\n"


/*     Globals     */
int **grid;     /* grid pointer */
int grid_size;  /* grid size */
int score;      /* score */
int score_last; /* Score for last move */
int score_high; /* Hiscore */
int printwidth; /* maximum length of any value in grid, for printing */


/*     Typedefs     */
typedef enum {
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
} dir_t;


/*     Macros     */
/* Repeat an expression y, x times */
#define ITER(x, expr)\
    do {\
        int i;\
        for (i = 0; i < (x); i++){ expr;}\
    } while (0)

/* Allocates a square pointer of array of arrays onto heap */
#define CALLOC2D(ptr, sz)\
    do {\
        int i;\
        ptr = calloc((sz), sizeof(*ptr));\
        for (i = 0; i < (sz); i++)\
            ptr[i] = calloc((sz), sizeof(*ptr));\
    } while (0)

/* Frees a square pointer of arrays to arrays */
#define FREE2D(ptr, sz)\
    do {\
        int i;\
        for (i = 0; i < (sz); i++)\
            free(ptr[i]);\
        free(ptr);\
    } while (0)

/* What occurs during a 'turn' of execution */
#define TURN(x) (gravitate(x) + merge(x) + gravitate(x))


/*     Functions     */
const char* get_highscore_file() {
    static char buffer[4096];
    if (getenv("XDG_DATA_HOME") != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/%s/%s", getenv("XDG_DATA_HOME"), DATADIR_NAME, HIGHSCORE_FILE_NAME);
    } else if (getenv("HOME") != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/.local/share/%s/%s", getenv("HOME"), DATADIR_NAME, HIGHSCORE_FILE_NAME);
    } else {
        return HIGHSCORE_FILE_NAME;
    }

    // create hierarrchy of directories up to highscore file location
    char* sep = strrchr(buffer, '/');
    while (sep != NULL) {
        *sep = '\0';
        if (strlen(buffer) != 0)
            mkdir(buffer, 0777);
        char* tmpsep = sep;
        sep = strrchr(buffer, '/');
        *tmpsep = '/';
    }
    return buffer;
}

/* Merges adjacent squares of the same value together in a certain direction */
int merge(dir_t d)
{
    int moved = 0;
    int i, j;

    if (d == DIR_LEFT) {
        /* Move from upper left, across rows until second to second-last elem each row */
        for (i = 0; i < grid_size; i++) {
            for (j = 0; j < grid_size - 1; j++) {
                if (grid[i][j] == grid[i][j + 1]) {
                    grid[i][j] <<= 1;
                    grid[i][j + 1] = 0;
                    score_last += grid[i][j];
                    score  += grid[i][j];
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_UP) {
        /* Move from upper left, across rows until final row which is skipped */
        for (i = 0; i < grid_size - 1; i++) {
            for (j = 0; j < grid_size; j++) {
                if (grid[i][j] == grid[i + 1][j]) {
                    grid[i][j] <<= 1;
                    grid[i + 1][j] = 0;
                    score_last += grid[i][j];
                    score  += grid[i][j];
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_RIGHT) {
        /* Move from lower right, backwards across rows until first elem each row */
        for (i = grid_size - 1; i >= 0; i--) {
            for (j = grid_size - 1; j > 0; j--) {
                if (grid[i][j] == grid[i][j - 1]) {
                    grid[i][j] <<= 1;
                    grid[i][j - 1] = 0;
                    score_last += grid[i][j];
                    score  += grid[i][j];
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_DOWN) {
        /* Move from lower right, across rows until first row which is skipped */
        for (i = grid_size - 1; i > 0; i--) {
            for (j = grid_size - 1; j >= 0; j--) {
                if (grid[i][j] == grid[i - 1][j]) {
                    grid[i][j] <<= 1;
                    grid[i - 1][j] = 0;
                    score_last += grid[i][j];
                    score  += grid[i][j];
                    moved = 1;
                }
            }
        }
    }

    return moved;
}


/* move all values in the grid to the edge given by the direction pressed */
/* would be nice to generalize this code a little so didn't need four loops */
/* if animations are wanted, then need to alter this so it moves each square one at a time */
int gravitate(dir_t d)
{
    int moved = 0;
    int i, j;

    if (d == DIR_LEFT) {
        for (i = 0; i < grid_size; i++) {
        for (j = 0; j < grid_size - 1; j++) {
            if (grid[i][j]) continue;
            int st = 1;
            while (j + st < grid_size && !grid[i][j + st]) st++;
            if (j + st < grid_size) {
                grid[i][j] = grid[i][j + st];
                grid[i][j + st] = 0;
                moved = 1;
            }
        }
        }
    }
    else if (d == DIR_UP) {
        for (i = 0; i < grid_size; i++) {
        for (j = 0; j < grid_size - 1; j++) {
            if (grid[j][i]) continue;
            int st = 1;
            while (j + st < grid_size && !grid[j + st][i]) st++;
            if (j + st < grid_size) {
                grid[j][i] = grid[j + st][i];
                grid[j + st][i] = 0;
                moved = 1;
            }
        }
        }
    }
    else if (d == DIR_RIGHT) {
        for (i = 0; i < grid_size; i++) {
        for (j = grid_size - 1; j > 0; j--) {
            if (grid[i][j]) continue;
            int st = 1;
            while (j - st >= 0 && !grid[i][j - st]) st++;
            if (j - st >= 0) {
                grid[i][j] = grid[i][j - st];
                grid[i][j - st] = 0;
                moved = 1;
            }
        }
        }
    }
    else if (d == DIR_DOWN) {
        for (i = 0; i < grid_size; i++) {
        for (j = grid_size - 1; j > 0; j--) {
            if (grid[j][i]) continue;
            int st = 1;
            while (j - st >= 0 && !grid[j - st][i]) st++;
            if (j - st >= 0) {
                grid[j][i] = grid[j - st][i];
                grid[j - st][i] = 0;
                moved = 1;
            }
        }
        }
    }
    return moved;
}

/* Return the current highscore */
int get_score_high()
{
    int s = 0;
    FILE *fd = fopen(HISCORE_FILE, "r");
    if (fd == NULL)
        fd = fopen(HISCORE_FILE, "w+");

    if (fscanf(fd, "%d", &s) == 1) {};
    fclose(fd);
    return s;
}

/* saves hiscore, but only if playing on standard size grid */
void save_score_high()
{
    if (score > score_high && grid_size == 4) {
        score_high = score;
        FILE *fd = fopen(HISCORE_FILE, "w+");
        fprintf(fd, "%d", score_high);
        fclose(fd);
    }
}

/* returns if there are any possible moves */
int moves_available()
{
    int i, j;
    for (i = 0; i < grid_size; i++)
    for (j = 0; j < grid_size; j++)
        if (!grid[i][j]
            || ((i + 1 < grid_size) && (grid[i][j] == grid[i + 1][j]))
            || ((j + 1 < grid_size) && (grid[i][j] == grid[i][j + 1])))
            return 1;
    return 0;
}

/* places a random block onto the grid - either a 4, or a 2 with a chance of 1:3 respectively */
/* could do this in a much smarter fashion by finding which spaces are free */
void rand_block()
{
    int x, y;
    while (grid[x = rand() % grid_size][y = rand() % grid_size]);
    grid[x][y] = (rand() & 3) ?  2 : 4;
}

/* quick floor log2(n) */
int flog2(unsigned int n)
{
    int k = 0;
    while (n)
        k++, n >>= 1;
    return k;
}

#ifndef NO_CURSES

/* draws the grid and fills it with the current values */
/* colors just rotate around, works for now, can be confusing when you have some fairly high values on the board */
void draw_grid(WINDOW *gamewin)
{
    // mvwprintw will sometimes have a useless arg, this is warned, but doesn't affect the program
    char *scr = score_last ? "SCORE: %d (+%d)\n" : "SCORE: %d\n";
    mvwprintw(gamewin, 0, 0, scr, score, score_last);
    mvwprintw(gamewin, 1, 0, "HISCR: %d\n", score_high);

    ITER(grid_size*(printwidth + 2) + 1, waddch(gamewin, '-'));
    int i, j, xps = 0, yps = 3;
    for (i = 0; i < grid_size; i++, xps = 0, yps++) {
        mvwprintw(gamewin, yps, xps++, "|");
        for (j = 0; j < grid_size; j++) {
            if (grid[i][j]) {
                wattron(gamewin, COLOR_PAIR(flog2(grid[i][j]) % 7));
                mvwprintw(gamewin, yps, xps, "%*d", printwidth, grid[i][j]);
                wattroff(gamewin, COLOR_PAIR(flog2(grid[i][j]) % 7));
                mvwprintw(gamewin, yps, xps + printwidth, " |");
            }
            else {
                ITER(printwidth + 1, waddch(gamewin, ' '));
                waddch(gamewin, '|');
            }
            xps += (printwidth + 2);
        }
    }
    ITER(grid_size*(printwidth + 2) + 1, waddch(gamewin, '-'));
    wrefresh(gamewin);
}

#else  /* ifndef NO_CURSES */

/* draws the grid and fills it with the current values */
void draw_grid()
{
    printf("HISCORE: %d |", score_high);
    printf("| SCORE: %d ", score);
    if (score_last) printf("(+%d)", score_last);
    printf("\n");

    // alter this grid_size + 1 to match abitrary grid size
    ITER(grid_size, printf("------"));
    printf("-\n");
    int i, j;
    for (i = 0; i < grid_size; i++) {
        printf("|");
        for (j = 0; j < grid_size; j++) {
            if (grid[i][j])
                printf("%*d |", 4, grid[i][j]);
            else
                printf("     |");
        }
        printf("\n");
    }
    ITER(grid_size, printf("------"));
    printf("-\n\n");
}

/* store the terminal settings and call this function on exit to restore */
struct termios sattr;
void reset_term()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &sattr);
}

#endif /* ifndef NO_CURSES */


/* entry point for the program */
/* parses options and stores the main game loop */
int main(int argc, char **argv)
{

    /* init variables */
    score      = 0;
    score_last = 0;
    score_high = get_score_high();
    grid_size  = DEFAULT_GRID_SIZE;
    printwidth = DEFAULT_GRID_SIZE;

    int n_blocks     = 1;

#ifndef NO_CURSES
    /* init ncurses environment */
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    int enable_color = has_colors();
#endif /* ifndef NO_CURSES */

    /* parse options */
    int c;
    while ((c = getopt(argc, argv, "rcChs:b:")) != -1) {
        switch (c) {
#ifndef NO_CURSES
        /* Color support */
        case 'c':
            enable_color = 1;
            break;
        case 'C':
            enable_color = 0;
            break;
#endif /* ifndef NO_CURSES */
        // different board sizes
        case 's':;
            int optint = strtol(optarg, NULL, 10);
            grid_size = optint > DEFAULT_GRID_SIZE ? optint : DEFAULT_GRID_SIZE;
            break;
        // different block spawn rate
        case 'b':
            n_blocks = strtol(optarg, NULL, 10);
            break;
        // reset hiscores
        case 'r':
#ifndef NO_CURSES
            endwin();
#endif /* ifndef NO_CURSES */
            printf("Are you sure you want to reset your highscores? (Y)es or (N)o\n");
            int response;
            if ((response = getchar()) == 'y' || response == 'Y') {
                FILE *fd = fopen(HISCORE_FILE, "w+");
                fclose(fd);
            }
            exit(EXIT_SUCCESS);
        // help menu
        case 'h':
#ifndef NO_CURSES
            endwin();
#endif /* ifndef NO_CURSES */
            printf(USAGE_STR);
            exit(EXIT_SUCCESS);
        }
    }

    /* Allocate memory once we actually know amount */
    CALLOC2D(grid, grid_size);

#ifndef NO_CURSES
    int width  = grid_size * (printwidth + 2) + 1;
    int height = grid_size * (printwidth + 2) + 3;

    if (enable_color) {
        if (!has_colors()) {
            fprintf(stderr, "Terminal does not support color\n");
        } else {
            start_color();
            init_pair(0, 1, 0);
            init_pair(1, 2, 0);
            init_pair(2, 3, 0);
            init_pair(3, 4, 0);
            init_pair(4, 5, 0);
            init_pair(5, 6, 0);
            init_pair(6, 7, 0);
        }
    }

    // might center in middle of screen
    WINDOW *gamewin = newwin(height, width, 1, 1);
    keypad(gamewin, TRUE);
#else  /* ifndef NO_CURSES */

    /* store term settings so we can restore on exit */
    tcgetattr(STDIN_FILENO, &sattr);
    atexit(reset_term);

    /* alters terminal stdin to not echo and doesn't need \n before reading getchar */
    struct termios tattr;
    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDOUT_FILENO, TCSANOW, &tattr);
#endif /* ifndef NO_CURSES */

    /* random seed */
    srand((unsigned int)time(NULL));
    ITER(2, rand_block());
#ifndef NO_CURSES
    draw_grid(gamewin);
#else  /* ifndef NO_CURSES */
    draw_grid();
#endif /* ifndef NO_CURSES */

    int key, moved;
    while (1) {
        /* will goto this if we didn't get a valid keypress */
        retry:;

        moved = 0;
        score_last = 0;

#ifndef NO_CURSES
        key = wgetch(gamewin);
#else  /* ifndef NO_CURSES */
        key = getchar();
#endif /* ifndef NO_CURSES */

        /* should check if anything changed during merge and if not retry */
        switch (key) {

        case 'h':
        case 'a':
#ifndef NO_CURSES
        case KEY_LEFT:
#endif /* ifndef NO_CURSES */
            moved = TURN(DIR_LEFT);
            break;

        case 'l':
        case 'd':
#ifndef NO_CURSES
        case KEY_RIGHT:
#endif /* ifndef NO_CURSES */
            moved = TURN(DIR_RIGHT);
            break;

        case 'j':
        case 's':
#ifndef NO_CURSES
        case KEY_DOWN:
#endif /* ifndef NO_CURSES */
            moved = TURN(DIR_DOWN);
            break;

        case 'k':
        case 'w':
#ifndef NO_CURSES
        case KEY_UP:
#endif /* ifndef NO_CURSES */
            moved = TURN(DIR_UP);
            break;

        case 'q':
            FREE2D(grid, grid_size);
#ifndef NO_CURSES
            erase();
            refresh();
            endwin();
#endif /* ifndef NO_CURSES */
            save_score_high();
            exit(EXIT_SUCCESS);

        default:
            goto retry;
        }

        if (!moves_available()) {
#ifndef NO_CURSES
            endwin();
#endif /* ifndef NO_CURSES */
            printf("\n"
                   "YOU LOSE! - Your score was %d\n", score);
            save_score_high();
            exit(EXIT_SUCCESS);
        }

        if (moved) {
            ITER(n_blocks, rand_block());
#ifndef NO_CURSES
            draw_grid(gamewin);
#else  /* ifndef NO_CURSES */
            draw_grid();
#endif /* ifndef NO_CURSES */
        }
    }

    return 0;
}
