#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char *homedir = getenv("HOME");
    if (homedir == NULL) {
        perror("getenv failed");
        return 1;
    }
    char *filename = "/.tpfile";

    size_t len = strlen(homedir) + strlen(filename) + 1;

    char *path = malloc(len);
    if (path == NULL) {
        perror("malloc failed");
        return 1;
    }

    snprintf(path, len, "%s%s", homedir, filename);
    printf("---\n");
    printf("file: 1 %s\n", filename);
    printf("file: 2 %s\n", path);

    FILE* datafile = fopen(path, "a");
    if (datafile == NULL) {
        perror("fopen failed");
        return 1;
    }

    fclose(datafile);
    free(path);

    return 0;
}