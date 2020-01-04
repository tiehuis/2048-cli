#include "gfx.h"
#include "merge.h"

#include <efi.h>
#include <efilib.h>

#define NUMBER_OF_COLORS 7

#define iterate(n, expression)\
    do {\
        int i;\
        for (i = 0; i < n; ++i) { expression; }\
    } while (0)

static void print_pos(INTN Row, INTN Column, CHAR16 *str)
{
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, Column, Row);
    uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, str);
}

struct gfx_state {
    INTN window_height, window_width;
};

INTN uefi_color_array[] = {
#ifdef INVERT_COLORS
    EFI_BLACK   | EFI_BACKGROUND_RED,
    EFI_BLACK   | EFI_BACKGROUND_GREEN,
    EFI_BLACK   | EFI_BACKGROUND_BROWN,
    EFI_BLACK   | EFI_BACKGROUND_BLUE,
    EFI_BLACK   | EFI_BACKGROUND_MAGENTA,
    EFI_BLACK   | EFI_BACKGROUND_CYAN,
    EFI_BLACK   | EFI_BACKGROUND_LIGHTGRAY,
#else
    EFI_RED     | EFI_BACKGROUND_BLACK,
    EFI_GREEN   | EFI_BACKGROUND_BLACK,
    EFI_YELLOW  | EFI_BACKGROUND_BLACK,
    EFI_BLUE    | EFI_BACKGROUND_BLACK,
    EFI_MAGENTA | EFI_BACKGROUND_BLACK,
    EFI_CYAN    | EFI_BACKGROUND_BLACK,
    EFI_WHITE   | EFI_BACKGROUND_BLACK,
#endif
};

struct gfx_state* gfx_init(struct gamestate *g)
{
    struct gfx_state *s = AllocatePool(sizeof(struct gfx_state));
    if (!s) return NULL;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->QueryMode, 4, ST->ConOut, ST->ConOut->Mode->Mode, &s->window_width, &s->window_height);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 0);
    uefi_call_wrapper(ST->ConOut->EnableCursor, 2, ST->ConOut, false);

    s->window_height = g->opts->grid_height * (g->print_width + 2) + 3;
    s->window_width  = g->opts->grid_width * (g->print_width + 2) + 1;

    return s;
}

void gfx_draw(struct gfx_state *s, struct gamestate *g)
{
    if (g->score_last) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 0);
        Print(L"Score: %ld (+%ld)     \n", g->score, g->score_last);
    } else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 0);
        Print(L"Score: %ld                  \n", g->score);
    }

    if (g->score >= g->score_high)
        g->score_high = g->score;

    Print(L"   Hi: %ld\n", g->score_high);

    //wattron(s->window, A_DIM);
    Print(L"%c", BOXDRAW_DOWN_RIGHT);
    iterate(g->opts->grid_width * (g->print_width + 2) + 1 - 2, Print(L"%c", (i+g->print_width+3) % (g->print_width+2) ? BOXDRAW_HORIZONTAL : BOXDRAW_DOWN_HORIZONTAL));
    Print(L"%c", BOXDRAW_DOWN_LEFT);
    //wattroff(s->window, A_DIM);

    int x, y,
        xpos = 0,
        ypos = 3;

    for (y = 0; y < g->opts->grid_height; ++y, ++ypos, xpos = 0) {
        //wattron(s->window, A_DIM);
        print_pos(ypos, xpos++, L"");
	Print(L"%c", BOXDRAW_VERTICAL);
        //wattroff(s->window, A_DIM);

        for (x = 0; x < g->opts->grid_width; ++x) {
            if (g->grid[x][y]) {
		if (g->opts->enable_color) {
		    UINTN attr = uefi_color_array[g->grid[x][y] % NUMBER_OF_COLORS];
		    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, attr);
		}
		uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, xpos, ypos);
                Print(L"%-*ld", g->print_width, merge_value(g->grid[x][y]));
                print_pos(ypos, xpos + g->print_width, L" ");
		if (g->opts->enable_color) {
		    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
		}

                //wattron(s->window, A_DIM);
                print_pos(ypos, xpos + g->print_width + 1, L"");
		Print(L"%c", BOXDRAW_VERTICAL);
                //wattroff(s->window, A_DIM);
            }
            else {
                //wattron(s->window, A_DIM);
                iterate(g->print_width + 1, Print(L" "));
                Print(L"%c", BOXDRAW_VERTICAL);
                //wattroff(s->window, A_DIM);
            }

            xpos += (g->print_width + 2);
        }
    }

    Print(L"\n");
    //wattron(s->window, A_DIM);
    Print(L"%c", BOXDRAW_UP_RIGHT);
    iterate(g->opts->grid_width * (g->print_width + 2) + 1 - 2, Print(L"%c", (i+g->print_width+3) % (g->print_width+2) ? BOXDRAW_HORIZONTAL : BOXDRAW_UP_HORIZONTAL));
    Print(L"%c", BOXDRAW_UP_LEFT);
    //wattroff(s->window, A_DIM);
    Print(L"\n");
}

int gfx_getch(struct gfx_state *s)
{
    EFI_INPUT_KEY Key;

    Pause();
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);

    /* Flush buffer */
    //nodelay(s->window, TRUE);
    //while (wgetch(s->window) != ERR);
    //nodelay(s->window, FALSE);

    switch (Key.ScanCode)
    {
	case SCAN_UP:
		return INPUT_UP;
		break;
	case SCAN_DOWN:
		return INPUT_DOWN;
		break;
	case SCAN_RIGHT:
		return INPUT_RIGHT;
		break;
	case SCAN_LEFT:
		return INPUT_LEFT;
		break;

	default:
		return Key.UnicodeChar;
		break;
	
    }
}

#define EFI_TIMER_PERIOD_MILLISECONDS(Milliseconds) MultU64x32((UINT64)(Milliseconds), 10000)

void gfx_sleep(int ms)
{
    EFI_STATUS Status;
    EFI_EVENT TimerEvent;
    UINTN Index;

    Status = uefi_call_wrapper(BS->CreateEvent, 5, EFI_EVENT_TIMER, 0, NULL, NULL, &TimerEvent);
    if(EFI_ERROR(Status)) {
	return;
    }

    Status = uefi_call_wrapper(BS->SetTimer, 3, TimerEvent, TimerRelative, EFI_TIMER_PERIOD_MILLISECONDS(ms));
    if(EFI_ERROR(Status)) {
	return;
    }

    Status = uefi_call_wrapper(BS->WaitForEvent, 3, 1, &TimerEvent, &Index);
    if(EFI_ERROR(Status)) {
	return;
    }
    Status = uefi_call_wrapper(BS->CloseEvent, TimerEvent);
}

void gfx_destroy(struct gfx_state *s)
{
    FreePool(s);
    uefi_call_wrapper(ST->ConOut->EnableCursor, 2, ST->ConOut, true);
}
