#ifndef BGEN_UTIL_FILE_H
#define BGEN_UTIL_FILE_H

#include <stdio.h>
#include <stdlib.h>

#include "bgen/bgen.h"
#include "variant_genotype.h"

struct BGenFile {
    byte *filepath;
    FILE *file;
    inti nvariants;
    inti nsamples;
    inti compression;
    inti layout;
    inti sample_ids_presence;
    inti samples_start;
    inti variants_start;
};

inti bgen_read(FILE *file, void *buffer, inti size);

#endif /* end of include guard: BGEN_UTIL_FILE_H */
