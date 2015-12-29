#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "engine.h"

const char *hs_dir_name  = "2048";
const char *hs_file_name = "highscore";

static const char* highscore_retrieve_file(void)
{
    static char buffer[4096];

    if (getenv("XDG_DATA_HOME") != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/%s/%s",
                getenv("XDG_DATA_HOME"), hs_dir_name, hs_file_name);
    }
    else if (getenv("HOME")) {
        snprintf(buffer, sizeof(buffer), "%s/.local/share/%s/%s",
                getenv("HOME"), hs_dir_name, hs_file_name);
    }
    else {
        return hs_file_name;
    }

    /* Create file only if it doesn't exist */
    if (access(buffer, F_OK) != -1)
        return buffer;

    char *sep = strrchr(buffer, '/');
    while (sep != NULL) {
        *sep = '\0';
        if (strlen(buffer) != 0)
            mkdir(buffer, 0777);
        char *tmpsep = sep;
        sep = strrchr(buffer, '/');
        *tmpsep = '/';
    }

    return buffer;
}

static inline void string_to_lower(char *str)
{
    for (; *str; ++str) *str = tolower(*str);
}

void highscore_reset(void)
{
    const char *hsfile = highscore_retrieve_file();
    const size_t resp_length = 16;
    char resp[resp_length];

    printf("Are you sure you want to reset your scores? Y(es) or N(o)\n");

    while (1) {
        /* fgets is used to avoid queuing that may occur with getchar */
        if (fgets(resp, resp_length, stdin) == NULL)
            return;

        string_to_lower(resp);

        const size_t sl = strlen(resp);
        if (sl < resp_length)
            resp[sl - 1] = '\0';

        if (!strncmp(resp, "yes", resp_length) || !strncmp(resp, "y", resp_length))
            goto reset_scores;
        else if (!strncmp(resp, "no", resp_length) || !strncmp(resp, "n",  resp_length))
            return;

        printf("Please enter Yes or No\n");
    }

reset_scores:;
    FILE *fd = fopen(hsfile, "w+");
    fprintf(fd, "%d", 0);
    fclose(fd);
}

long highscore_load(struct gamestate *g)
{
    const char *hsfile = highscore_retrieve_file();
    long result = 0;

    FILE *fd = fopen(hsfile, "r");
    if (fd == NULL)
        fd = fopen(hsfile, "w+");

    if (fd == NULL) {
        fprintf(stderr, "load: Failed to open highscore file\n");
        return 0;
    }

    if (fscanf(fd, "%ld", &result) != 1) {
        fprintf(stderr, "load: Failed to parse highscore file\n");
        result = 0;
    }

    fclose(fd);

    if (g) g->score_high = result;
    return result;
}

void highscore_save(struct gamestate *g)
{
    /* Someone could make their own merge rules for highscores and this could be meaningless,
     * howeverhighscores are in plaintext, so that isn't that much of a concern */
    if (g->score < g->score_high || g->opts->grid_width != 4 ||
            g->opts->grid_height != 4 || g->opts->ai == true)
        return;

    const char *hsfile = highscore_retrieve_file();

    FILE *fd = fopen(hsfile, "w");
    if (fd == NULL) {
        fprintf(stderr, "save: Failed to open highscore file\n");
        return;
    }

    if (fprintf(fd, "%ld", g->score) < 0) {
        fprintf(stderr, "save: Failed to write highscore file\n");
    }
    fclose(fd);
}
