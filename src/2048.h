#ifndef _2048_H_
#define _2048_H_

typedef enum {
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT
} dir_t;

// Repeat an expression y, x times */
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

/* Constants */
#define DEFAULT_GRID_SIZE 4
#define HISCORE_FILE ".hs2048g"
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
   "    -c              Enables color support (ncurses version only)\n"

#endif
