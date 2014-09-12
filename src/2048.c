/*
 * 2048.c - Main version that should be worked on.
 *
 * Things to do:
 *  - Finish cleaning up code.
 *  - Ask user if they would like to play again instead of exiting immediately.
 *  - Add an actual goal (what happens when we reach 2048?).
 *  - Keep track list of past scores, and store in a better fashion independent
 *    of cwd. Also, store in not in plaintext.
 * */

#include <stdlib.h> /* for malloc  */
#include <time.h>   /* for time    */
#include <unistd.h> /* for getopts */
#include <ncurses.h> 
#include "2048.h"

int **grid;     /* grid pointer */
int grid_size;  /* grid size */
int score;      /* score */
int score_last; /* Score for last move */
int score_high; /* Hiscore */
int printwidth; /* maximum length of any value in grid, for printing */

/* Merges adjacent squares of the same value together in a certain direction */
int merge(dir_t d)
{
    int moved = 0;
    int i, j;

    if (d == DIR_LEFT || d == DIR_RIGHT) {
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
    else {
        for (i = 0; i < grid_size - 1; i++) {
        for (j = 0; j < grid_size; j++) {
            if (grid[i][j] == grid[i + 1][j]) {
                grid[i][j] <<= 2;
                grid[i + 1][j] = 0;
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
int get_score_high() {
    int s = 0;
    FILE *fd = fopen(HISCORE_FILE, "r");
    if (fd == NULL) 
        fd = fopen(HISCORE_FILE, "w+");

    fscanf(fd, "%d", &s);
    fclose(fd);
    return s;
}

/* saves hiscore, but only if playing on standard size grid */
void save_score_high() {
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
    char *scr = score_last ? "SCORE: %d (+%d)\n" : "SCORE: %d\n";
    mvwprintw(gamewin, 0, 0, scr, score, score_last);
    mvwprintw(gamewin, 1, 0, "HISCR: %d\n", score_high);

    ITER(grid_size*(printwidth + 2) + 1, waddch(gamewin, '-')); 
    int i, j, xps = 0, yps = 3;
    for (i = 0; i < grid_size; i++, xps = 0, yps++) {
        mvwprintw(gamewin, yps, xps++, "|");
        for (j = 0; j < grid_size; j++) {
            if (grid[i][j]) {
                wattron(gamewin, COLOR_PAIR(flog2(grid[i][j]) & 7));
                mvwprintw(gamewin, yps, xps, "%*d", printwidth, grid[i][j]);
                wattroff(gamewin, COLOR_PAIR(flog2(grid[i][j]) & 7));
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
    score      = 0;
    score_last = 0;
    score_high = get_score_high();
    grid_size  = DEFAULT_GRID_SIZE;
    printwidth = DEFAULT_GRID_SIZE;

    int n_blocks = 1;
    
    /* parse options */
    int c;
    while ((c = getopt(argc, argv, "rchs:b:")) != -1) {
        switch (c) {
        /* Color support */
        case 'c':
            if (has_colors()) {
                start_color();
                init_pair(0, 1, 0);
                init_pair(1, 2, 0);
                init_pair(2, 3, 0);
                init_pair(3, 4, 0);
                init_pair(4, 5, 0);
                init_pair(5, 6, 0);
                init_pair(6, 7, 0);
            }
            else {
                fprintf(stderr, "Terminal does not support color\n");
            }
            break;
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
            endwin();
            printf("Are you sure you want to reset your highscores? (Y)es or (N)o\n");
            int response;
            if ((response = getchar()) == 'y' || response == 'Y') {
                FILE *fd = fopen(HISCORE_FILE, "w+");
                fclose(fd);
            }
            exit(EXIT_SUCCESS);
        // help menu
        case 'h':
            endwin();
            printf(USAGE_STR);
            exit(EXIT_SUCCESS);
        }
    }
    
    /* Allocate memory once we actually know amount */
    CALLOC2D(grid, grid_size);

    int width  = grid_size * (printwidth + 2) + 1;
    int height = grid_size * (printwidth + 2) + 3;

    // might center in middle of screen
    WINDOW *gamewin = newwin(height, width, 1, 1);
    keypad(gamewin, TRUE);

    /* random seed */
    srand((unsigned int)time(NULL));
    ITER(2, rand_block());
    draw_grid(gamewin);

    int key, moved;
    while (1) {
        /* will goto this if we didn't get a valid keypress */
retry:;
        moved = 0;
        score_last = 0;
        key = wgetch(gamewin);

        /* should check if anything changed during merge and if not retry */
        switch (key) {
        case 'h':
        case 'a':
        case KEY_LEFT:
            moved = TURN(DIR_LEFT);
            break;
        case 'l':
        case 'd':
        case KEY_RIGHT:
            moved = TURN(DIR_RIGHT);
            break;
        case 'j':
        case 's':
        case KEY_DOWN:
            moved = TURN(DIR_DOWN);
            break;
        case 'k':
        case 'w':
        case KEY_UP:
            moved = TURN(DIR_UP);
            break;
        case 'q':
            FREE2D(grid, grid_size);
            erase();
            refresh();
            endwin();
            save_score_high();
            exit(EXIT_SUCCESS);
        default:
            goto retry;
        }
        
        if (!moves_available()) {
            endwin();
            printf("\n"
                   "YOU LOSE! - Your score was %d\n", score);
            save_score_high();
            exit(EXIT_SUCCESS);
        }

        if (moved) {
            ITER(n_blocks, rand_block());
            draw_grid(gamewin);
        }
    }

    return 0;
}
