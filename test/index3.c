#include "bgen.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

int main() {

    size_t i, j, ii, jj;
    const char filename[] = "data/complex.23bits.bgen";
    struct bgen_file *bgen;
    int nsamples, nvariants;
    double *probabilities;

    if ((bgen = bgen_open(filename)) == NULL)
        return 1;

    if (bgen_create_variants_metadata_file3(bgen, "complex_index3.index", 1))
        return 1;

    bgen_close(bgen);

    return 0;
}
