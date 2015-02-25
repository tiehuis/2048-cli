#include "ai.h"
#include "engine.h"

const char moves[] = {'w', 'a', 's', 'd'};

int ai_move(struct gamestate *g)
{
    if (g->opts->interactive) usleep(50000);
    return moves[rand() % 4];
}
