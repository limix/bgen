#include "bgen.h"
#include "cass.h"
#include <stdlib.h>

inline static int imin(int a, int b) { return a < b ? a : b; }

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
    bgen_free_index(vi);
    bgen_close(bgen);

    bgen = bgen_open("data/example.14bits.bgen");
    variants = bgen_read_variants_metadata(bgen, &vi, 0);

    bgen_store_variants_metadata(bgen, variants, vi, "assert_interface_2.metadata1");
    bgen_free_variants_metadata(bgen, variants);
    bgen_free_index(vi);
    bgen_close(bgen);

    free(probs);

    return 0;
}
