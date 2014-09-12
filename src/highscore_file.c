#include "2048.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

const char* get_highscore_file() {
    static char buffer[4096];
    if (getenv("XDG_DATA_HOME") != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/%s/%s", getenv("XDG_DATA_HOME"), DATADIR_NAME, HIGHSCORE_FILE_NAME);
    } else if (getenv("HOME") != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/.local/share/%s/%s", getenv("HOME"), DATADIR_NAME, HIGHSCORE_FILE_NAME);
    } else {
        return HIGHSCORE_FILE_NAME;
    }

    // create hierarrchy of directories up to highscore file location
    char* sep = strrchr(buffer, '/');
    while (sep != NULL) {
        *sep = '\0';
        if (strlen(buffer) != 0)
            mkdir(buffer, 0777);
        char* tmpsep = sep;
        sep = strrchr(buffer, '/');
        *tmpsep = '/';
    }
    return buffer;
}
