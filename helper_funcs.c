#include <stdio.h>
#include <stdlib.h>
#include "helper_funcs.h"

unsigned char *GetFileContents(char *file_name, long *out_fsize) {
    FILE *file = fopen(file_name, "rb");

    if (file == NULL) {
        printf("Unable to open file.");
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    unsigned char *buffer = malloc(fsize);

    fread(buffer, fsize, 1, file);
    fclose(file);

    *out_fsize = fsize;

    return buffer;
}
