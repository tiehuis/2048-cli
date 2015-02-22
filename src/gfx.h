#ifndef GFX_H
#define GFX_H

#include "engine.h"

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
