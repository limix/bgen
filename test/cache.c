#include "bgen/bgen.h"

int store_index(const byte *bgenfp, const byte *cachefp) {
    struct BGenFile *bgen;
    struct BGenVar *variants;
    struct BGenVI *index;

    if ((bgen = bgen_open(bgenfp)) == NULL)
        return 1;

    if ((variants = bgen_read_variants(bgen, &index)) == NULL)
        return 1;

    if (bgen_store_variants(bgen, variants, index, cachefp))
        return 1;

    bgen_free_variants(bgen, variants);
    bgen_free_index(index);

    bgen_close(bgen);

    return 0;
}
