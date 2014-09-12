/*
 * 2048_no_curses.c - Non-curses version that can be updated at a later time 
 **/

#include <stdio.h>  /* for printf  */
#include <stdlib.h> /* for malloc  */
#include <termios.h>/*             */
#include <time.h>   /* for time    */
#include <unistd.h> /* for getopts */
#include "2048.h"

int **g; /* grid pointer */
int SZ; /* grid size */
int s;  /* Current score */
int sl; /* Score for last turn */
int hs; /* Highscore */

/* Merges adjacent squares of the same value together in a certain direction */
int merge(int d)
{
    int moved = 0;
    if (d == DIR_LEFT) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ; j++) {
                if (j + 1 < SZ && g[i][j] && g[i][j] == g[i][j + 1]) {
                    g[i][j] <<= 1;
                    sl += g[i][j];
                    s  += g[i][j];
                    g[i][j++ + 1] = 0;
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_UP) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ; j++) {
                if (j + 1 < SZ && g[j][i] && g[j][i] == g[j + 1][i]) {
                    g[j][i] <<= 1;
                    sl += g[j][i];
                    s  += g[j][i];
                    g[j++ + 1][i] = 0;
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_RIGHT) {
        int i, j;
        for (i = SZ - 1; i >= 0; i--) {
            for (j = SZ - 1; j >= 0; j--) {
                if (j > 0 && g[i][j] && g[i][j] == g[i][j - 1]) {
                    g[i][j] <<= 1;
                    sl += g[i][j];
                    s  += g[i][j];
                    g[i][j-- - 1] = 0;
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_DOWN) {
        int i, j;
        for (i = SZ - 1; i >= 0; i--) {
            for (j = SZ - 1; j >= 0; j--) {
                if (j > 0 && g[j][i] && g[j][i] == g[j - 1][i]) {
                    g[j][i] <<= 1;
                    sl += g[j][i];
                    s  += g[j][i];
                    g[j-- - 1][i] = 0;
                    moved = 1;
                }
            }
        }
    }
    return moved;
}

    
/* move all values in the grid to the edge given by the direction pressed */
/* would be nice to generalize this code a little so didn't need four loops */
int gravitate(int d)
{
    int moved = 0;
    if (d == DIR_LEFT) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ - 1; j++) {
                if (g[i][j]) continue;
                int st = 1;
                while (j + st < SZ && !g[i][j + st]) st++;
                if (j + st < SZ) {
                    g[i][j] = g[i][j + st];
                    g[i][j + st] = 0;
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_UP) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = 0; j < SZ - 1; j++) {
                if (g[j][i]) continue;
                int st = 1;
                while (j + st < SZ && !g[j + st][i]) st++;
                if (j + st < SZ) {
                    g[j][i] = g[j + st][i];
                    g[j + st][i] = 0;
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_RIGHT) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = SZ - 1; j > 0; j--) {
                if (g[i][j]) continue;
                int st = 1;
                while (j - st >= 0 && !g[i][j - st]) st++;
                if (j - st >= 0) {
                    g[i][j] = g[i][j - st];
                    g[i][j - st] = 0;
                    moved = 1;
                }
            }
        }
    }
    else if (d == DIR_DOWN) {
        int i, j;
        for (i = 0; i < SZ; i++) {
            for (j = SZ - 1; j > 0; j--) {
                if (g[j][i]) continue;
                int st = 1;
                while (j - st >= 0 && !g[j - st][i]) st++;
                if (j - st >= 0) {
                    g[j][i] = g[j - st][i];
                    g[j - st][i] = 0;
                    moved = 1;
                }
            }
        }
    }
    return moved;
}

/* loads hiscore */
void load_score() {
    FILE *fd = fopen(HISCORE_FILE, "r");
    if (fd == NULL) fd = fopen(HISCORE_FILE, "w+");
    if (fscanf(fd, "%d", &hs) == EOF) hs = 0;
    fclose(fd);
}

/* saves hiscore, but only if playing on standard size grid */
void save_score() {
    if (s > hs && SZ == 4) {
        hs = s;
        FILE *fd = fopen(HISCORE_FILE, "w+");
        fprintf(fd, "%d", hs);
        fclose(fd);
    }
}

/* returns if there are any possible moves */
int moves_available()
{
    int i, j;
    for (i = 0; i < SZ; i++)
        for (j = 0; j < SZ; j++)
            if (!g[i][j] || ((i + 1 < SZ) && (g[i][j] == g[i + 1][j])) || ((j + 1 < SZ) && (g[i][j] == g[i][j + 1])))
                return 1;
    return 0;
}

/* places a random block onto the grid - either a 4, or a 2 with a ratio of 1:3 respectively */
/* do this in a smarter fashion */
void rand_block()
{
    int x_p, y_p;
    while (g[x_p = rand() % SZ][y_p = rand() % SZ]);
    g[x_p][y_p] = (rand() & 3) ?  2 : 4;
}

/* draws the grid and fills it with the current values */
void draw_grid()
{
    printf("HISCORE: %d |", hs);
    printf("| SCORE: %d ", s);
    if (sl) printf("(+%d)", sl); 
    printf("\n"); 

    // alter this SZ + 1 to match abitrary grid size
    ITER(SZ, printf("------"));
    printf("-\n");
    int i, j;
    for (i = 0; i < SZ; i++) {
        printf("|");
        for (j = 0; j < SZ; j++) {
            if (g[i][j])
                printf("%*d |", 4, g[i][j]);
            else
                printf("     |");
        }
        printf("\n");
    }
    ITER(SZ, printf("------"));
    printf("-\n\n");
}

/* store the terminal settings and call this function on exit to restore */
struct termios sattr;
void reset_term()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &sattr);
}

/* entry point for the program */
/* parses options and stores the main game loop */
int main(int argc, char **argv)
{
 
    /* init variables */
    hs = 0;
    s  = 0;
    sl = 0;
    SZ = 4;
    CALLOC2D(g, SZ);

    load_score();
    int n_blocks = 1;
    
    /* parse options */
    int c;
    while ((c = getopt(argc, argv, "rhs:b:")) != -1) {
        switch (c) {
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
                printf("Are you sure you want to reset your highscores? (Y)es or (N)o\n");
                int response;
                if ((response = getchar()) == 'y' || response == 'Y') {
                    FILE *fd = fopen(HISCORE_FILE, "w+");
                    fclose(fd);
                }
                exit(EXIT_SUCCESS);
            case 'h':
                printf(USAGE_STR);
                exit(EXIT_SUCCESS);
        }
    }

    /* random seed */
    srand((unsigned int)time(NULL));
    ITER(2, rand_block());
    draw_grid();

    /* store term settings so we can restore on exit */
    tcgetattr(STDIN_FILENO, &sattr);
    atexit(reset_term);

    /* alters terminal stdin to not echo and doesn't need \n before reading getchar */
    struct termios tattr;
    tcgetattr(STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDOUT_FILENO, TCSANOW, &tattr);

    int key, moved;
    while (1) {
        /* will goto this if we didn't get a valid keypress */
        retry:;
        moved = 0;
        key = getchar();
        sl = 0;

        /* should check if anything changed during merge and if not retry */
        switch (key) {
            case 'h':
            case 'a':
                moved = TURN(DIR_LEFT);
                break;
            case 'l':
            case 'd':
                moved = TURN(DIR_RIGHT);
                break;
            case 'j':
            case 's':
                moved = TURN(DIR_DOWN);
                break;
            case 'k':
            case 'w':
                moved = TURN(DIR_UP);
                break;
            case 'q':
                save_score();
                exit(EXIT_SUCCESS);
            default:
                goto retry;
        }
        
        if (!moves_available()) {
            printf("\n"
                   "YOU LOSE! - Your score was %d\n", s);
            save_score();
            exit(EXIT_SUCCESS);
        }

        if (moved) {
            ITER(n_blocks, rand_block());
            draw_grid();
        }
    }
}
