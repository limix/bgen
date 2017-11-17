#include "bgen/bgen.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

const byte eg32bits[] = "test/data/example.32bits.bgen";
const byte eg32bits_idx[] = "test/data/example.32bits.bgen.index";

int test_store(const byte *fp0, const byte *fp1) {
    BGenFile *bgen;
    Variant *variants;
    VariantIndexing *index;

    bgen = bgen_open(fp0);
    if (bgen == NULL)
        return FAIL;

    variants = bgen_read_variants(bgen, &index);
    if (variants == NULL)
        return FAIL;

    if (bgen_store_variants(bgen, variants, fp1) == FAIL)
        return FAIL;

    bgen_free_variants(bgen, variants);
    bgen_free_indexing(index);

    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int test_load(const byte *fp0, const byte *fp1) {
    BGenFile *bgen;
    Variant *variants;
    VariantIndexing *index;

    bgen = bgen_open(fp0);
    if (bgen == NULL)
        return FAIL;

    variants = bgen_load_variants(bgen, eg32bits_idx, &index);
    if (variants == NULL)
        return FAIL;

    bgen_free_variants(bgen, variants);
    bgen_free_indexing(index);

    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int main() {
    if (test_store(eg32bits, eg32bits_idx) == FAIL)
        return FAIL;

    if (test_load(eg32bits, eg32bits_idx) == FAIL)
        return FAIL;

    return SUCCESS;
}
