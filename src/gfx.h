#ifndef GFX_H
#define GFX_H

#include "engine.h"

/* These can be defined by an implemenation if special codes are used
 * to represent some keys. e.g. KEY_LEFT in ncurses */
#define GFX_RIGHT_EXTRA
#define GFX_LEFT_EXTRA
#define GFX_DOWN_EXTRA
#define GFX_UP_EXTRA

struct gfx_state;

/* Initialization of a graphics context */
struct gfx_state* gfx_init(struct gamestate *);

/* Drawing of a game_state onto a graphics context */
void gfx_draw(struct gfx_state *, struct gamestate *);

/* Blocking get character. Should not be buffered for best results */
int gfx_getch(struct gfx_state *);

/* Destruction of a graphics context */
void gfx_destroy(struct gfx_state *);

/* Sleep for a specifed millisecond period */
void gfx_sleep(int ms);

#endif
