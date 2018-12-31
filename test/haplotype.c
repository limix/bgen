#include "bgen.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

    size_t i, j, ii, jj;
    const char filename[] = "data/haplotypes.bgen";
    struct bgen_file *bgen;
    int nsamples, nvariants;

    if ((bgen = bgen_open(filename)) == NULL)
        return 1;

    if ((nsamples = bgen_nsamples(bgen)) != 4)
        return 1;

    if ((nvariants = bgen_nvariants(bgen)) != 4)
        return 1;

    struct bgen_str *sample_ids = bgen_read_samples(bgen, 0);

    if (strncmp("sample_0", sample_ids[0].str, sample_ids[0].len) != 0)
        return 1;

    bgen_free_samples(bgen, sample_ids);

    struct bgen_vi *index;
    struct bgen_var *variants = bgen_read_metadata(bgen, &index, 0);

    if (strncmp("RS1", variants[0].rsid.str, variants[0].rsid.len) != 0)
        return 1;

    if (variants[0].nalleles != 2)
        return 1;

    for (i = 0; i < nvariants; ++i) {
        if (strncmp("A", variants[i].allele_ids[0].str,
                    variants[i].allele_ids[0].len) != 0)
            return 1;

        if (strncmp("G", variants[i].allele_ids[1].str,
                    variants[i].allele_ids[1].len) != 0)
            return 1;
    }

    if (bgen_max_nalleles(index) != 2)
        return 1;

    bgen_free_variants_metadata(bgen, variants);

    bgen_close(bgen);

    struct bgen_vg *vg = bgen_open_variant_genotype(index, 1);

    if (bgen_ncombs(vg) != 4)
        return 1;

    if (bgen_ploidy(vg, 0) != 2)
        return 1;

    if (bgen_min_ploidy(vg) != 2)
        return 1;

    if (bgen_max_ploidy(vg) != 2)
        return 1;

    if (bgen_nalleles(vg) != 2)
        return 1;

    if (bgen_phased(vg) != 1)
        return 1;

    bgen_close_variant_genotype(vg);

    double real_probs[] = {
        1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0};

    jj = 0;
    for (i = 0; i < nvariants; ++i) {
        vg = bgen_open_variant_genotype(index, i);

        double *probabilities = malloc(nsamples * bgen_ncombs(vg) * sizeof(double));

        bgen_read_variant_genotype(index, vg, probabilities);

        for (ii = 0; ii < nsamples; ++ii) {
            for (j = 0; j < bgen_ncombs(vg); ++j) {
                if (probabilities[ii * bgen_ncombs(vg) + j] != real_probs[jj])
                    return 1;
                jj++;
            }
        }
        bgen_close_variant_genotype(vg);
        free(probabilities);
    }

    bgen_free_index(index);

    return 0;
}
