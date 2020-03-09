#include "bgen/bgen.h"
#include "cass.h"
#include <stdlib.h>

int main()
{
    struct bgen_str str = {0, NULL};
    cass_equal_int(str.length, 0);

    struct bgen_file *bgen = bgen_file_open("nexist");
    cass_cond(bgen == NULL);

    bgen = bgen_file_open("data/example.v11.bgen");

    cass_equal_int(bgen_file_nsamples(bgen), 500);
    cass_equal_int(bgen_file_nvariants(bgen), 199);
    cass_equal_int(bgen_file_contain_samples(bgen), 0);

    struct bgen_str *samples = bgen_file_read_samples(bgen, 0);
    cass_cond(samples == NULL);

    bgen_file_close(bgen);

    bgen = bgen_file_open("data/example.14bits.bgen");
    cass_cond(bgen != NULL);

    cass_equal_int(bgen_file_nsamples(bgen), 500);
    cass_equal_int(bgen_file_nvariants(bgen), 199);
    cass_equal_int(bgen_file_contain_samples(bgen), 1);

    samples = bgen_file_read_samples(bgen, 0);
    cass_cond(samples != NULL);
    bgen_free_samples(bgen, samples);

    bgen_file_close(bgen);

    bgen = bgen_file_open("data/example.14bits.bgen");
    struct bgen_vi *vi;
    struct bgen_mf* mf = bgen_create_metafile(bgen, "example.14bits.bgen.metadata", 1, 0);
    int nvariants = 0;
    struct bgen_vm *vm = bgen_read_partition(mf, 0, &nvariants);

    cass_equal_int(vm[0].position, 2000);
    cass_equal_int(vm[0].nalleles, 2);
    cass_cond(bgen_str_equal(BGEN_STR("RSID_2"), vm[0].rsid));
    cass_cond(bgen_str_equal(BGEN_STR("SNPID_2"), vm[0].id));
    cass_cond(bgen_str_equal(BGEN_STR("01"), vm[0].chrom));
    cass_cond(bgen_str_equal(BGEN_STR("A"), vm[0].allele_ids[0]));

    bgen_free_partition(vm, nvariants);
    bgen_close_metafile(mf);
    bgen_file_close(bgen);

    bgen = bgen_file_open("data/example.14bits.bgen");
    mf = bgen_open_metafile("example.14bits.bgen.metadata");
    vm = bgen_read_partition(mf, 0, &nvariants);
    struct bgen_vg *vg = bgen_open_genotype(bgen, vm[3].vaddr);

    cass_equal_int(bgen_nalleles(vg), 2);
    cass_equal_int(bgen_missing(vg, 3), 0);
    cass_equal_int(bgen_ploidy(vg, 3), 2);
    cass_equal_int(bgen_min_ploidy(vg), 2);
    cass_equal_int(bgen_max_ploidy(vg), 2);
    cass_equal_int(bgen_ncombs(vg), 3);
    cass_equal_int(bgen_phased(vg), 0);

    double *probs = malloc(500 * 3 * sizeof(double));
    bgen_read_genotype(bgen, vg, probs);
    cass_close(probs[0], 0.00488311054141488121);
    cass_close(probs[1], 0.02838308002197399704);
    cass_close(probs[2], 0.96673380943661113562);
    cass_close(probs[3], 0.99047793444424092613);

    bgen_free_partition(vm, nvariants);
    bgen_close_genotype(vg);
    bgen_close_metafile(mf);
    bgen_file_close(bgen);

    return cass_status();
}
