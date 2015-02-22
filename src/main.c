#include <stdio.h>
#include <stdbool.h>
#include "engine.h"
#include "gfx.h"

void draw_then_sleep(struct gfx_state *s, struct gamestate *g)
{
    gfx_draw(s, g);
    gfx_sleep(40);
}

int main(int argc, char **argv)
{
    struct gameoptions *o = gameoptions_default();
    struct gamestate *g   = gamestate_init(parse_options(o, argc, argv));
    struct gfx_state *s   = gfx_init(g);

    int game_running = true;
    while (game_running) {
        gfx_draw(s, g);

get_new_key:;
        int direction = dir_invalid;
        switch (gfx_getch(s)) {
            case 'h':
            case 'a':
                direction = dir_left;
                break;
            case 'l':
            case 'd':
                direction = dir_right;
                break;
            case 'j':
            case 's':
                direction = dir_down;
                break;
            case 'k':
            case 'w':
                direction = dir_up;
                break;
            case 'q':
                game_running = false;
                break;
            default:
                goto get_new_key;
        }

        /* Game will only end if 0 moves available */
        if (game_running) {
            gamestate_tick(s, g, direction, g->opts->animate ? draw_then_sleep : NULL);

            int spawned;
            for (spawned = 0; spawned < g->opts->spawn_rate; spawned++)
                gamestate_new_block(g);

            if (gamestate_end_condition(g)) {
                game_running = false;
            }
        }
    }

    gfx_destroy(s);
    printf("Highscore: %ld\n", g->score_high);
    printf("    Score: %ld\n", g->score);
    gamestate_clear(g);
    return 0;
}
