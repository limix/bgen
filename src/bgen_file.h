#ifndef BGEN_BGEN_FILE_H
#define BGEN_BGEN_FILE_H

#include <stdio.h>

struct bgen_file {
    char *filepath;
    FILE *file;
    int nvariants;
    int nsamples;
    int compression;
    int layout;
    int sample_ids_presence;
    int samples_start;
    int variants_start;
};

#endif /* end of include guard: BGEN_BGEN_FILE_H */
