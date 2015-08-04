#include <stdlib.h>
#include "ai.h"
#include "engine.h"
#include "gfx.h"

const char moves[] = {'w', 'a', 's', 'd'};

int ai_move(struct gamestate *g)
{
    /* Ensure srand is called somewhere prior */
    if (g->opts->interactive) gfx_sleep(50);
    return moves[rand() % 4];
}
