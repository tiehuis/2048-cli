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

void highscore_reset(void)
{
    const char *hsfile = highscore_retrieve_file();

    FILE *fd = fopen(hsfile, "w+");
    fprintf(fd, "%d", 0);
    fclose(fd);
}

void highscore_load(struct gamestate *g)
{
    const char *hsfile = highscore_retrieve_file();

    FILE *fd = fopen(hsfile, "r");
    if (fd == NULL)
        fd = fopen(hsfile, "w+");

    fscanf(fd, "%ld", &g->score_high);
    fclose(fd);
}

void highscore_save(struct gamestate *g)
{
    if (g->score < g->score_high || g->opts->grid_width != 4 || g->opts->grid_height != 4)
        return;

    const char *hsfile = highscore_retrieve_file();

    FILE *fd = fopen(hsfile, "w");
    fprintf(fd, "%ld", g->score);
    fclose(fd);
}
