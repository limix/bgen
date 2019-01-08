/*
 * DEPRECATED functions for metadata for bgen index versions 01 and 02.
 *
 * Please, use the functions in the `meta.h` file instead.
 *
 * Reading variants metadata (and generating the variants index) can be costly
 * as it requires accessing chunks of data across the file. We therefore
 * provide the functions
 *     - bgen_store_variants_metadata
 *     - bgen_load_variants_metadata
 *     - bgen_create_variants_metadata_file
 * for storing and reading that information from an additional file. We refer
 * to this file as variants metadata file.
 *
 * Note: remember to call `bgen_free_variants_metadata` and `bgen_free_index`
 * after use.
 */
#ifndef BGEN_DEPR_META_H
#define BGEN_DEPR_META_H

#include "api.h"
#include "file.h"
#include "geno.h"
#include "str.h"

/* Variant metadata. DEPRECATED: Please, use bgen_vm instead. */
struct bgen_var
{
    struct bgen_str id;
    struct bgen_str rsid;
    struct bgen_str chrom;
    int position;
    int nalleles;
    struct bgen_str *allele_ids;
};

BGEN_DEPRECATED_API struct bgen_var *
bgen_read_variants_metadata(struct bgen_file *, struct bgen_vi **, int);
BGEN_DEPRECATED_API void bgen_free_variants_metadata(const struct bgen_file *,
                                                     struct bgen_var *);
BGEN_DEPRECATED_API void bgen_free_index(struct bgen_vi *);
/* Store variants metadata. */
BGEN_DEPRECATED_API int bgen_store_variants_metadata(const struct bgen_file *,
                                                     struct bgen_var *,
                                                     struct bgen_vi *, const char *);
/* Read variants metadata from file. */
BGEN_DEPRECATED_API struct bgen_var *
bgen_load_variants_metadata(const struct bgen_file *, const char *, struct bgen_vi **,
                            int);
/* Create a variants metadata file.

    Helper for easy creation of variants metadata file.

    Note: this file is not part of the bgen file format specification.
*/
BGEN_DEPRECATED_API int bgen_create_variants_metadata_file(const char *, const char *,
                                                           int);
/* Get the maximum number of alleles across the entire file. */
BGEN_DEPRECATED_API int bgen_max_nalleles(struct bgen_vi *);

#endif /* BGEN_DEPR_META_H */
