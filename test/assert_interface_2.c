#include "bgen.h"
#include "cass.h"
#include <stdlib.h>

inline static int imin(int a, int b) { return a < b ? a : b; }

#if 0
struct bgen_string {
    int len;
    char *str;
};

struct bgen_file; /* bgen file handler */
struct bgen_vi;   /* variant index */
struct bgen_vg;   /* variant genotype */

/* Variant metadata. */
struct bgen_var {
    struct bgen_string id;
    struct bgen_string rsid;
    struct bgen_string chrom;
    int position;
    int nalleles;
    struct bgen_string *allele_ids;
};

/* Open a file and return a bgen file handler. */
BGEN_API struct bgen_file *bgen_open(const char *filepath);
/* Close a bgen file handler. */
BGEN_API void bgen_close(struct bgen_file *bgen);

/* Get the number of samples. */
BGEN_API int bgen_nsamples(const struct bgen_file *bgen);
/* Get the number of variants. */
BGEN_API int bgen_nvariants(const struct bgen_file *bgen);
/* Check if the file contains sample identifications. */
BGEN_API int bgen_sample_ids_presence(const struct bgen_file *bgen);

/* Get array of sample identifications. */
BGEN_API struct bgen_string *bgen_read_samples(struct bgen_file *bgen, int verbose);
/* Free array of sample identifications. */
BGEN_API void bgen_free_samples(const struct bgen_file *bgen,
                                struct bgen_string *samples);

/* Read variants metadata and generate variants index.
    Reading variants metadata (and generating the variants index) can be costly
    as it requires accessing chunks of data across the file. We therefore
    provide the functions
        - bgen_store_variants_metadata
        - bgen_load_variants_metadata
        - bgen_create_variants_metadata_file
    for storing and reading that information from an additional file. We refer
    to this file as variants metadata file.
    Note: remember to call `bgen_free_variants_metadata` and `bgen_free_index`
   after use.
*/
BGEN_API struct bgen_var *bgen_read_variants_metadata(struct bgen_file *bgen,
                                                      struct bgen_vi **vi, int verbose);
BGEN_API void bgen_free_variants_metadata(const struct bgen_file *bgen,
                                          struct bgen_var *variants);
BGEN_API void bgen_free_index(struct bgen_vi *vi);

/* Get the maximum number of alleles across the entire file. */
BGEN_API int bgen_max_nalleles(struct bgen_vi *vi);

/* Open a variant for genotype queries.
    A variant genotype handler is needed to call
        - bgen_read_variant_genotype
        - bgen_nalleles
        - bgen_ploidy
        - bgen_ncombs
    Note: remember to call `bgen_close_variant_genotype` after use.
 */
BGEN_API struct bgen_vg *bgen_open_variant_genotype(struct bgen_vi *vi, size_t index);
/* Close a variant genotype handler. */
BGEN_API void bgen_close_variant_genotype(struct bgen_vi *vi, struct bgen_vg *vg);

/* Read the probabilities of each possible genotype.
    The variant index and a variant genotype handler are required.
    The number of probabilities can be found from `bgen_ncombs`.
*/
BGEN_API void bgen_read_variant_genotype(struct bgen_vi *vi, struct bgen_vg *vg,
                                         double *probs);
/* Get the number of alleles. */
BGEN_API int bgen_nalleles(const struct bgen_vg *vg);
/* Return 1 if variant is missing for the sample; 0 otherwise. */
BGEN_API int bgen_missing(const struct bgen_vg *vg, size_t index);
/* Get the ploidy. */
BGEN_API int bgen_ploidy(const struct bgen_vg *vg, size_t index);
/* Get the minimum ploidy of the variant. */
BGEN_API int bgen_min_ploidy(const struct bgen_vg *vg);
/* Get the maximum ploidy of the variant. */
BGEN_API int bgen_max_ploidy(const struct bgen_vg *vg);
/* Get the number of genotype combinations. */
BGEN_API int bgen_ncombs(const struct bgen_vg *vg);
/* Return 1 for phased or 0 for unphased genotype. */
BGEN_API int bgen_phased(const struct bgen_vg *vg);

/* Store variants metadata. */
BGEN_API int bgen_store_variants_metadata(const struct bgen_file *bgen,
                                          struct bgen_var *variants, struct bgen_vi *vi,
                                          const char *filepath);
/* Read variants metadata from file. */
BGEN_API struct bgen_var *bgen_load_variants_metadata(const struct bgen_file *bgen,
                                                      const char *filepath,
                                                      struct bgen_vi **vi, int verbose);
/* Create a variants metadata file.
    Helper for easy creation of variants metadata file.
    Note: this file is not part of the bgen file format specification.
*/
BGEN_API int bgen_create_variants_metadata_file(const char *bgen_fp, const char *vi_fp,
int verbose);
#endif

int main()
{

    struct bgen_string str = {0, NULL};
    assert_equal_int(str.len, 0);
    struct bgen_var var = {{0, NULL}, {0, NULL}, {0, NULL}, 15, 13, NULL};

    assert_equal_int(var.position, 15);
    assert_equal_int(var.nalleles, 13);

    struct bgen_file *bgen = bgen_open("nexist");
    assert_null(bgen);
    bgen_close(bgen);

    bgen = bgen_open("data/example.v11.bgen");

    assert_equal_int(bgen_nsamples(bgen), 500);
    assert_equal_int(bgen_nvariants(bgen), 199);
    assert_equal_int(bgen_sample_ids_presence(bgen), 0);

    struct bgen_string *samples = (struct bgen_string *)bgen_read_samples(bgen, 0);
    assert_null(samples);
    bgen_free_samples(bgen, (struct bgen_str *)samples);

    bgen_close(bgen);

    bgen = bgen_open("data/example.14bits.bgen");

    assert_equal_int(bgen_nsamples(bgen), 500);
    assert_equal_int(bgen_nvariants(bgen), 199);
    assert_equal_int(bgen_sample_ids_presence(bgen), 1);

    samples = (struct bgen_string *)bgen_read_samples(bgen, 0);
    assert_not_null(samples);
    bgen_free_samples(bgen, (struct bgen_str *)samples);

    bgen_close(bgen);

    bgen = bgen_open("data/example.14bits.bgen");
    struct bgen_vi *vi;
    struct bgen_var *variants = bgen_read_variants_metadata(bgen, &vi, 0);

    assert_equal_int(variants[0].position, 2000);
    assert_equal_int(variants[0].nalleles, 2);
    assert_strncmp(variants[0].rsid.str, "RSID_2", imin(variants[0].rsid.len, 6));
    assert_strncmp(variants[0].id.str, "SNPID_2", imin(variants[0].id.len, 7));
    assert_strncmp(variants[0].chrom.str, "01", imin(variants[0].chrom.len, 2));
    assert_strncmp(variants[0].allele_ids[0].str, "A",
                   imin(variants[0].allele_ids[0].len, 1));
    bgen_free_variants_metadata(bgen, variants);
    assert_equal_int(bgen_max_nalleles(vi), 2);
    bgen_free_index(vi);
    bgen_close(bgen);

    bgen = bgen_open("data/example.14bits.bgen");
    variants = bgen_read_variants_metadata(bgen, &vi, 0);
    struct bgen_vg *vg = bgen_open_variant_genotype(vi, 3);

    assert_equal_int(bgen_nalleles(vg), 2);
    assert_equal_int(bgen_missing(vg, 3), 0);
    assert_equal_int(bgen_ploidy(vg, 3), 2);
    assert_equal_int(bgen_min_ploidy(vg), 2);
    assert_equal_int(bgen_max_ploidy(vg), 2);
    assert_equal_int(bgen_ncombs(vg), 3);
    assert_equal_int(bgen_phased(vg), 0);

    double *probs = malloc(500 * 3 * sizeof(double));
    bgen_read_variant_genotype(vi, vg, probs);
    assert_almost_equal(probs[0], 0.00488311054141488121);
    assert_almost_equal(probs[1], 0.02838308002197399704);
    assert_almost_equal(probs[2], 0.96673380943661113562);
    assert_almost_equal(probs[3], 0.99047793444424092613);

    bgen_free_variants_metadata(bgen, variants);
    bgen_close_variant_genotype(vi, vg);
    bgen_close(bgen);

    bgen = bgen_open("data/example.14bits.bgen");
    variants = bgen_read_variants_metadata(bgen, &vi, 0);

    bgen_store_variants_metadata(bgen, variants, vi, "assert_interface_2.metadata1");
    bgen_free_variants_metadata(bgen, variants);
    bgen_close(bgen);

    bgen = bgen_open("data/example.14bits.bgen");
    variants =
        bgen_load_variants_metadata(bgen, "assert_interface_2.metadata1", &vi, 0);
    bgen_free_variants_metadata(bgen, variants);
    bgen_close(bgen);

    bgen_create_variants_metadata_file("data/example.32bits.bgen",
                                       "assert_interface_2.metadata1", 0);

    return 0;
}
