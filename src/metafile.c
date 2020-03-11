#include "metafile.h"
#include "bgen/bgen.h"
#include "bmath.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "mem.h"
#include "metafile_write.h"
#include "partition.h"
#include "pbar.h"
#include "report.h"
#include "str.h"
#include <assert.h>

static struct bgen_mf* bgen_mf_alloc(char const* filepath);
static uint32_t        partition_nvariants(struct bgen_mf const* mf, uint32_t partition);

struct bgen_mf* bgen_metafile_create(struct bgen_file* bgen, char const* filepath,
                                     unsigned npartitions, int verbose)
{
    struct bgen_mf* mf = bgen_mf_alloc(filepath);
    mf->npartitions = npartitions;
    mf->nvariants = bgen_file_nvariants(bgen);

    if (!(mf->stream = fopen(filepath, "w+b"))) {
        bgen_perror("could not create file %s", filepath);
        goto err;
    }

    if (write_metafile_header(mf->stream))
        goto err;

    if (write_metafile_nvariants(mf->stream, mf->nvariants))
        goto err;

    if (bgen_fseek(mf->stream, sizeof(uint64_t), SEEK_CUR))
        goto err;

    if (bgen_file_seek_variants_start(bgen))
        goto err;

    mf->partition_offset = malloc(sizeof(uint64_t) * (mf->npartitions + 1));

    if (write_metafile_metadata_block(mf->stream, mf->partition_offset, mf->npartitions,
                                      mf->nvariants, bgen, verbose))
        goto err;

    if (write_metafile_offsets_block(mf->stream, mf->npartitions, mf->partition_offset))
        goto err;

    if (fflush(mf->stream)) {
        bgen_perror("could not fflush metafile");
        goto err;
    }

    return mf;
err:
    bgen_metafile_close(mf);
    return NULL;
}

struct bgen_mf* bgen_metafile_open(const char* filepath)
{
    struct bgen_mf* mf = bgen_mf_alloc(filepath);

    if (!(mf->stream = fopen(mf->filepath, "rb"))) {
        bgen_perror("Could not open %s", mf->filepath);
        goto err;
    }

    char header[13];
    if (fread(header, 13 * sizeof(char), 1, mf->stream) != 1) {
        bgen_perror("Could not fetch the metafile header");
        goto err;
    }

    if (strncmp(header, "bgen index 03", 13)) {
        bgen_error("Unrecognized bgen index version: %.13s", header);
        goto err;
    }

    if (fread(&(mf->nvariants), sizeof(uint32_t), 1, mf->stream) != 1) {
        bgen_perror("Could not read the number of variants from metafile");
        goto err;
    }

    if (fread(&(mf->metadata_size), sizeof(uint64_t), 1, mf->stream) != 1) {
        bgen_perror("Could not read the metasize from metafile");
        goto err;
    }

    if (mf->metadata_size > INT64_MAX) {
        bgen_error("`metadata_size` overflow");
        goto err;
    }

    if (bgen_fseek(mf->stream, (int64_t)mf->metadata_size, SEEK_CUR)) {
        bgen_error("Could to fseek to the number of partitions");
        goto err;
    }

    if (fread(&(mf->npartitions), sizeof(uint32_t), 1, mf->stream) != 1) {
        bgen_perror("Could not read the number of partitions");
        goto err;
    }

    mf->partition_offset = malloc(mf->npartitions * sizeof(uint64_t));

    for (size_t i = 0; i < mf->npartitions; ++i) {
        if (fread(mf->partition_offset + i, sizeof(uint64_t), 1, mf->stream) != 1) {
            bgen_perror("Could not read partition offsets");
            goto err;
        }
    }

    return mf;
err:
    bgen_metafile_close(mf);
    return NULL;
}

uint32_t bgen_metafile_npartitions(struct bgen_mf const* mf) { return mf->npartitions; }

uint32_t bgen_metafile_nvariants(struct bgen_mf const* mf) { return mf->nvariants; }

struct bgen_partition* bgen_metafile_read_partition2(struct bgen_mf const* mf, uint32_t index)
{
    FILE* file = mf->stream;

    if (index >= mf->npartitions) {
        bgen_error("the provided partition number %d is out-of-range", index);
        return NULL;
    }

    uint32_t               nvariants = partition_nvariants(mf, index);
    struct bgen_partition* partition = bgen_partition_create(nvariants);

    if (bgen_fseek(file, 13 + 4 + 8, SEEK_SET)) {
        bgen_perror("could not fseek metafile");
        goto err;
    }

    if (mf->partition_offset[index] > INT64_MAX) {
        bgen_error("`partition_offset` overflow");
        goto err;
    }

    if (bgen_fseek(file, (int64_t)mf->partition_offset[index], SEEK_CUR)) {
        bgen_perror("could not fseek metafile");
        goto err;
    }

    for (uint32_t i = 0; i < partition_nvariants(mf, index); ++i) {
        struct bgen_variant* vm = bgen_variant_create();

        if (fread_ui64(file, &vm->genotype_offset, 8))
            goto err;

        if ((vm->id = bgen_str_fread(file, 2)) == NULL)
            goto err;

        if ((vm->rsid = bgen_str_fread(file, 2)) == NULL)
            goto err;

        if ((vm->chrom = bgen_str_fread(file, 2)) == NULL)
            goto err;

        if (fread_ui32(file, &vm->position, 4))
            goto err;

        if (fread_ui16(file, &vm->nalleles, 2))
            goto err;

        bgen_variant_create_alleles(vm, vm->nalleles);

        for (uint16_t j = 0; j < vm->nalleles; ++j) {
            if ((vm->allele_ids[j] = bgen_str_fread(file, 4)) == NULL)
                goto err;
        }

        bgen_partition_set(partition, i, vm);
    }

    return partition;

err:
    bgen_partition_destroy(partition);
    return NULL;
}

int bgen_metafile_close(struct bgen_mf* mf)
{
    if (mf) {
        if (fclose(mf->stream)) {
            bgen_perror("could not close the %s file", mf->filepath);
            return 1;
        }
        mf->stream = NULL;
        free_c(mf->filepath);
        free_c(mf->partition_offset);
        free_c(mf);
    }
    return 0;
}

static struct bgen_mf* bgen_mf_alloc(char const* filepath)
{
    struct bgen_mf* mf = malloc(sizeof(struct bgen_mf));
    mf->filepath = strdup(filepath);
    mf->stream = NULL;
    mf->partition_offset = NULL;
    return mf;
}

static uint32_t partition_nvariants(struct bgen_mf const* mf, uint32_t partition)
{
    uint32_t size = ceildiv_uint32(mf->nvariants, mf->npartitions);
    return min_uint32(size, mf->nvariants - size * partition);
}
