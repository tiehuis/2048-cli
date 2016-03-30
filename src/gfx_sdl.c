#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "merge.h"
#include "gfx.h"

/* Side length of a 'pixel' in pixels */
#define TTF_FONT_PT 32

#define iterate(n, expression)\
    do {\
        int i;\
        for (i = 0; i < n; ++i) { expression; }\
    } while (0)

struct gfx_state {
    SDL_Window *window;
    SDL_Surface *surface;
    TTF_Font *font;
    int side_length;
    int window_height;
    int window_width;
};

struct gfx_state* gfx_init(struct gamestate *g)
{
    struct gfx_state *s = malloc(sizeof(struct gfx_state));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    s->font = TTF_OpenFont(TTF_FONT_PATH, TTF_FONT_PT);

    s->side_length   = TTF_FontLineSkip(s->font);
    s->window_height = g->opts->grid_height * (g->print_width + 2) + 3;
    s->window_width  = g->opts->grid_width * (g->print_width + 2) + 1;
    s->window = SDL_CreateWindow(
                "2048",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,

                /* Spacing is inconsistent right now. Need to find width and
                 * height spacing to accurately resize window */
                s->window_width * TTF_FONT_PT * 0.7,
                s->window_height * s->side_length * 0.35,
                SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
            );

    s->surface = SDL_GetWindowSurface(s->window);

    return s;
}

#define rect_set(r, xv, yv, wv, hv)\
    do {\
        r.x = xv; r.y = yv; r.w = wv; r.h = hv;\
    } while (0)


void gfx_draw(struct gfx_state *s, struct gamestate *g)
{
    /* This shouldn't ever overflow. Max width effectively is determined by the size
     * of two integers' text representation */
    const int buffer_length = 64;
    char string_buffer[buffer_length];

    /* Set up text object so we can write to sdl window */
    SDL_Surface *text;
    SDL_Color text_color = {255, 255, 255, 0};
    SDL_Rect rect;

    /* Clear screen */
    SDL_FillRect(s->surface, NULL, SDL_MapRGB(s->surface->format, 0, 0, 0));

    if (g->score_last)
        snprintf(string_buffer, buffer_length, "Score: %ld (+%ld)", g->score, g->score_last);
    else
        snprintf(string_buffer, buffer_length, "Score: %ld", g->score);

    rect_set(rect, 0, 0, 0, 0);
    text = TTF_RenderText_Solid(s->font, string_buffer, text_color);
    SDL_BlitSurface(text, NULL, s->surface, &rect);

    snprintf(string_buffer, buffer_length, "   Hi: %ld", g->score_high);
    rect_set(rect, 0, s->side_length * 1, 0, 0);
    text = TTF_RenderText_Solid(s->font, string_buffer, text_color);
    SDL_BlitSurface(text, NULL, s->surface, &rect);

    int i;
    for (i = 0; i < g->opts->grid_width * (g->print_width + 2) + 1; ++i)
        string_buffer[i] = '-';
    string_buffer[i] = '\0';

    rect_set(rect, 0, s->side_length * 2, 0, 0);
    text = TTF_RenderText_Solid(s->font, string_buffer, text_color);
    SDL_BlitSurface(text, NULL, s->surface, &rect);

    int x, y;
    char line_buffer[buffer_length];
    for (y = 0; y < g->opts->grid_height; ++y) {
        int line_index = 0;
        line_buffer[line_index++] = '|';

        for (x = 0; x < g->opts->grid_width; ++x) {
            if (g->grid[x][y]) {
                snprintf(string_buffer, buffer_length, "%*ld |", g->print_width, merge_value(g->grid[x][y]));
                strncpy(line_buffer + line_index, string_buffer, buffer_length - line_index);
                line_index += strlen(string_buffer);
            }
            else {
                snprintf(string_buffer, buffer_length, "%*s |", g->print_width, "");
                strncpy(line_buffer + line_index, string_buffer, buffer_length - line_index);
                line_index += strlen(string_buffer);
            }
        }

        line_buffer[line_index] = 0;
        rect_set(rect, 0, s->side_length * (y + 3), 0, 0);
        text = TTF_RenderText_Solid(s->font, line_buffer, text_color);
        SDL_BlitSurface(text, NULL, s->surface, &rect);
    }

    for (i = 0; i < g->opts->grid_height * (g->print_width + 2) + 1; ++i)
        string_buffer[i] = '-';
    string_buffer[i] = '\0';

    rect_set(rect, 0, s->side_length * (y + 3), 0, 0);
    text = TTF_RenderText_Solid(s->font, string_buffer, text_color);
    SDL_BlitSurface(text, NULL, s->surface, &rect);
    SDL_UpdateWindowSurface(s->window);
}

/* This getch we parse here, and we just return when we get an appropriate
 * event */
int gfx_getch(struct gfx_state *s)
{
    (void)s; /* Supress unused warning */
    SDL_Event event;

    while (1) {
        while (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return 'q';
            case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    return 'q';
                case SDLK_UP:
                case SDLK_w:
                    return 'w';
                case SDLK_DOWN:
                case SDLK_s:
                    return 's';
                case SDLK_LEFT:
                case SDLK_a:
                    return 'a';
                case SDLK_RIGHT:
                case SDLK_d:
                    return 'd';
                default:
                     break;
            }
                break;
            default:
                break;
            }
        }
    }
}

void gfx_sleep(int ms)
{
    SDL_Delay(ms);
}

void gfx_destroy(struct gfx_state *s)
{
    SDL_DestroyWindow(s->window);
    TTF_Quit();
    SDL_Quit();
}
