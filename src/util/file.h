#ifndef BGEN_UTIL_FILE_H
#define BGEN_UTIL_FILE_H

#include <stdio.h>
#include <stdlib.h>

#include "bgen/bgen.h"

typedef struct VariantGenotype VariantGenotype;
typedef struct VariantIndexing VariantIndexing;

typedef struct BGenFile {
    byte *filepath;
    FILE *file;
    inti nvariants;
    inti nsamples;
    inti compression;
    inti layout;
    inti sample_ids_presence;
    inti samples_start;
    inti variants_start;
} BGenFile;

inti bgen_read(FILE *file, void *buffer, inti size);

#endif /* end of include guard: BGEN_UTIL_FILE_H */
