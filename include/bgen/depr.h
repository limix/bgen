/** Deprecated functions.
 * @file bgen/depr.h
 *
 * @deprecated Please, use the functions found in the other files instead.
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
 * Note: remember to call @ref bgen_free_variants_metadata and @ref bgen_free_index
 * after use.
 */
#ifndef BGEN_DEPR_H
#define BGEN_DEPR_H

#include "api.h"
#include "file.h"
#include "geno.h"
#include "str.h"
#include <stddef.h>

/** Variant metadata.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:type:`bgen_vm` instead.
 * \endrst
 */
struct bgen_var
{
    struct bgen_str id;          /**< variant identification. */
    struct bgen_str rsid;        /**< rsid */
    struct bgen_str chrom;       /**< chromossome nane */
    int position;                /**< base-pair position */
    int nalleles;                /**< number of alleles */
    struct bgen_str *allele_ids; /**< allele ids */
};

struct bgen_vi; /* variant index */
struct bgen_vg; /* variant genotype */

/** Open a variant for genotype queries.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_open_genotype` instead.
 * \endrst
 * */
BGEN_DEPRECATED_API struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi,
                                                               size_t index);
/** Read the probabilities of each possible genotype.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_read_genotype` instead.
 * \endrst
 * */
BGEN_DEPRECATED_API int bgen_read_variant_genotype(struct bgen_vi *, struct bgen_vg *,
                                                   double *);
/** Close a variant genotype handler.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_close_genotype` instead.
 * \endrst
 * */
BGEN_DEPRECATED_API void bgen_close_variant_genotype(struct bgen_vi *,
                                                     struct bgen_vg *);
/** Check if the file contains sample identifications.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_contain_samples` instead.
 * \endrst
 * */
BGEN_DEPRECATED_API int bgen_sample_ids_presence(const struct bgen_file *bgen);

/** Read variants metadata and index.
 *
 * Alternatively, the user can call
 * @ref bgen_load_variants_metadata to read that information from a file
 * created by calling @ref bgen_store_variants_metadata.
 *
 * Remember to call @ref bgen_free_variants_metadata on the returned array
 * to release allocated resources after the interaction has finished.
 *
 * \rst
 * .. seealso::
 *
 *     - Use :cpp:func:`bgen_free_variants_metadata` to release resources
 *       associated with variants metadata.
 *     - Use :cpp:func:`bgen_free_index` to release resources associated
 *       with the index.
 * \endrst
 *
 * @param bgen Bgen file handler.
 * @param vi Variants index.
 * @param verbose: `1` to show progress; `0` to disable output.
 * @return Variants information.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_read_partition` instead.
 * \endrst
 */
BGEN_DEPRECATED_API struct bgen_var *
bgen_read_variants_metadata(struct bgen_file *bgen, struct bgen_vi **vi, int verbose);
/** Free memory associated with variants metadata.
 *
 * @param bgen Bgen file handler.
 * @param variants Variants information.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_free_partition` instead.
 * \endrst
 */
BGEN_DEPRECATED_API void bgen_free_variants_metadata(const struct bgen_file *bgen,
                                                     struct bgen_var *variants);
/** Free memory associated with variants index.
 *
 * @param vi Variants index.
 * \rst
 * .. deprecated:: 3.0.0
 * \endrst
 *
 */
BGEN_DEPRECATED_API void bgen_free_index(struct bgen_vi *vi);
/** Save variants metadata into a file for faster reloading.
 *
 *
 * @param bgen Bgen file handler.
 * @param variants Variants metadata.
 * @param vi Variants index.
 * @param filepath File path to the variants metadata.
 * @return `0` on success; `1` otherwise.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_create_metafile` instead.
 * \endrst
 */
BGEN_DEPRECATED_API int bgen_store_variants_metadata(const struct bgen_file *bgen,
                                                     struct bgen_var *variants,
                                                     struct bgen_vi *vi,
                                                     const char *filepath);
/** Read variants metadata from file.
 *
 * Load variants metadata from a file.
 *
 * @param bgen bgen file handler.
 * @param filepath File path to the variants metadata.
 * @param vi Variants index.
 * @param verbose `1` to show progress; `0` to disable output.
 * @return Variants metadata.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_open_metafile` instead.
 * \endrst
 */
BGEN_DEPRECATED_API struct bgen_var *
bgen_load_variants_metadata(const struct bgen_file *bgen, const char *filepath,
                            struct bgen_vi **vi, int verbose);
/** Create a variants metadata file.
 *
 * Helper for easy creation of variants metadata file.
 *
 * Create variants index and save it to a file.
 *
 * @param bgen_fp File path to a bgen file.
 * @param vi_fp File path to the destination file.
 * @param verbose `1` to show progress or `0` to disable output.
 * @return `0` on success; `1` otherwise.
 *
 * Note: this file is not part of the bgen file format specification.
 * \rst
 * .. deprecated:: 3.0.0
 *     Please, use :cpp:func:`bgen_create_metafile` instead.
 * \endrst
 */
BGEN_DEPRECATED_API int
bgen_create_variants_metadata_file(const char *bgen_fp, const char *vi_fp, int verbose);
/** Get the maximum number of alleles across the entire file.
 *
 * @param vi Variant index.
 * return Maximum number of alleles.
 * \rst
 * .. deprecated:: 3.0.0
 * \endrst
 */
BGEN_DEPRECATED_API int bgen_max_nalleles(struct bgen_vi *vi);

#endif /* BGEN_DEPR_H */
