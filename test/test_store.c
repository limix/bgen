#include "example_files.h"

#include <math.h>
#include <stdio.h>
#include <string.h>


int test_store(const byte *fp0, const byte *fp1) {
    struct BGenFile *bgen;
    struct BGVar *variants;
    struct BGenVI *index;

    bgen = bgen_open(fp0);
    if (bgen == NULL)
        return FAIL;

    variants = bgen_read_variants(bgen, &index);
    if (variants == NULL)
        return FAIL;

    if (bgen_store_variants(bgen, variants, index, fp1) == FAIL)
        return FAIL;

    bgen_free_variants(bgen, variants);
    bgen_free_index(index);

    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int test_load(const byte *fp0, const byte *fp1) {
    struct BGenFile *bgen;
    struct BGVar *variants;
    struct BGenVI *index;

    bgen = bgen_open(fp0);
    if (bgen == NULL)
        return FAIL;

    variants = bgen_load_variants(bgen, fp1, &index);
    if (variants == NULL)
        return FAIL;

    bgen_free_variants(bgen, variants);
    bgen_free_index(index);

    bgen_close(bgen);

    return EXIT_SUCCESS;
}

int main() {
    inti nexamples, prec, i;
    const byte *ex, *ix;

    nexamples = get_nexamples();

    for (i = 0; i < nexamples; ++i) {
        ex = get_example(i);
        ix = get_example_index(i);
        prec = get_example_precision(i);

        if (test_store(ex, ix) == FAIL)
            return FAIL;

        if (test_load(ex, ix) == FAIL)
            return FAIL;
    }

    return SUCCESS;
}
