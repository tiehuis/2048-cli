#include "engine.h"

#include <efi.h>
#include <efilib.h>

EFI_GUID EFI_2048_GUID =
    { 0x6F633F38, 0x2169, 0x42FF, {0xA2, 0x1D, 0x66, 0x34, 0x65, 0xCF, 0x3C, 0x71} };
CHAR16 *hs_file_name = L"highscore";

void highscore_reset(void)
{
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;

    Print(L"Are you sure you want to reset your scores? Y(es) or N(o)\n");

    while (1) {
	Pause();
	Status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
	if (EFI_ERROR(Status))
	    return;

	if (Key.ScanCode == 0x00) {
	    if (Key.UnicodeChar == 'y' || Key.UnicodeChar == 'Y') {
		goto reset_scores;
	    } else if (Key.UnicodeChar == 'n' || Key.UnicodeChar == 'N') {
		return;
	    }
	}

        Print(L"Please enter Y or N\n");
    }

reset_scores:;
    LibDeleteVariable(hs_file_name, &EFI_2048_GUID);
}

long highscore_load(struct gamestate *g)
{
    long result = 0;
    long *result_ptr;
    size_t result_size;

    result_ptr = LibGetVariableAndSize(hs_file_name, &EFI_2048_GUID, &result_size);
    if (result_ptr != NULL) {
	if(result_size == sizeof(result)) {
	    result = *result_ptr;
	}
	FreePool(result_ptr);
    }

    if (g) g->score_high = result;
    return result;
}

void highscore_save(struct gamestate *g)
{
    EFI_STATUS Status;

    /* Someone could make their own merge rules for highscores and this could be meaningless,
     * howeverhighscores are in plaintext, so that isn't that much of a concern */
    if (g->score < g->score_high || g->opts->grid_width != 4 ||
            g->opts->grid_height != 4 || g->opts->ai == true)
        return;

    Status = LibSetNVVariable(hs_file_name, &EFI_2048_GUID, sizeof(g->score), &g->score);
    if (EFI_ERROR(Status))
	Print(L"save: Failed to write highscore file\n");
}
