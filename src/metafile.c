#include "metafile.h"
#include "bgen/bgen.h"
#include "bmath.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "mem.h"
#include "metafile_write.h"
#include "pbar.h"
#include "report.h"
#include "str.h"
#include "variant.h"
#include <assert.h>

static struct bgen_mf* bgen_mf_alloc(char const* filepath);
static uint32_t        partition_nvariants(struct bgen_mf const* mf, uint32_t partition);

struct bgen_mf* bgen_metafile_create(struct bgen_file* bgen, char const* filepath,
                                     unsigned npartitions, int verbose)
{
    struct bgen_mf* mf = bgen_mf_alloc(filepath);
    mf->idx.npartitions = npartitions;
    mf->idx.nvariants = bgen_file_nvariants(bgen);

    if (!(mf->stream = fopen(filepath, "w+b"))) {
        bgen_perror("could not create file %s", filepath);
        goto err;
    }

    if (write_metafile_header(mf->stream))
        goto err;

    if (write_metafile_nvariants(mf->stream, mf->idx.nvariants))
        goto err;

    if (LONG_SEEK(mf->stream, sizeof(uint64_t), SEEK_CUR))
        goto err;

    if (bgen_file_seek_variants_start(bgen))
        goto err;

    mf->idx.poffset = malloc(sizeof(uint64_t) * (mf->idx.npartitions + 1));

    if (write_metafile_metadata_block(mf->stream, &mf->idx, bgen, verbose))
        goto err;

    if (write_metafile_offsets_block(mf->stream, mf->idx.npartitions, mf->idx.poffset))
        goto err;

    if (fflush(mf->stream)) {
        bgen_perror("could not fflush metafile");
        goto err;
    }

    return mf;
err:
    bgen_mf_close(mf);
    return NULL;
}

struct bgen_mf* bgen_open_metafile(const char* filepath)
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

    if (fread(&(mf->idx.nvariants), sizeof(uint32_t), 1, mf->stream) != 1) {
        bgen_perror("Could not read the number of variants from metafile");
        goto err;
    }

    if (fread(&(mf->idx.metasize), sizeof(uint64_t), 1, mf->stream) != 1) {
        bgen_perror("Could not read the metasize from metafile");
        goto err;
    }

    if (mf->idx.metasize > OFF_T_MAX) {
        bgen_error("fseeking would cause failure");
        goto err;
    }

    if (LONG_SEEK(mf->stream, (OFF_T)mf->idx.metasize, SEEK_CUR)) {
        bgen_error("Could to fseek to the number of partitions");
        goto err;
    }

    if (fread(&(mf->idx.npartitions), sizeof(uint32_t), 1, mf->stream) != 1) {
        bgen_perror("Could not read the number of partitions");
        goto err;
    }

    mf->idx.poffset = malloc(mf->idx.npartitions * sizeof(uint64_t));

    for (size_t i = 0; i < mf->idx.npartitions; ++i) {
        if (fread(mf->idx.poffset + i, sizeof(uint64_t), 1, mf->stream) != 1) {
            bgen_perror("Could not read partition offsets");
            goto err;
        }
    }

    return mf;
err:
    bgen_mf_close(mf);
    return NULL;
}

uint32_t bgen_metafile_npartitions(struct bgen_mf const* mf) { return mf->idx.npartitions; }

uint32_t bgen_metafile_nvariants(struct bgen_mf const* mf) { return mf->idx.nvariants; }

struct bgen_vm* bgen_read_partition(struct bgen_mf const* mf, uint32_t partition,
                                    uint32_t* nvariants)
{
    struct bgen_vm* vars = NULL;
    FILE*           file = mf->stream;

    if (partition >= mf->idx.npartitions) {
        bgen_error("The provided partition number %d is out-of-range", partition);
        goto err;
    }

    *nvariants = partition_nvariants(mf, partition);
    vars = malloc((*nvariants) * sizeof(struct bgen_vm));
    for (uint32_t i = 0; i < *nvariants; ++i)
        init_metadata(vars + i);

    if (LONG_SEEK(file, 13 + 4 + 8, SEEK_SET)) {
        bgen_perror("Could not fseek bgen index file");
        goto err;
    }

    if (mf->idx.poffset[partition] > OFF_T_MAX)
        bgen_die("offset overflow");

    if (LONG_SEEK(file, (OFF_T)mf->idx.poffset[partition], SEEK_CUR)) {
        bgen_perror("Could not fseek bgen index file");
        goto err;
    }

    for (uint32_t i = 0; i < *nvariants; ++i) {
        fread_long(file, &vars[i].vaddr, 8);

        vars[i].id = bgen_str_fread(file, 2);
        vars[i].rsid = bgen_str_fread(file, 2);
        vars[i].chrom = bgen_str_fread(file, 2);

        fread_ui32(file, &vars[i].position, 4);
        fread_ui16(file, &vars[i].nalleles, 2);
        vars[i].allele_ids = malloc(sizeof(struct bgen_str*) * vars[i].nalleles);

        for (int j = 0; j < vars[i].nalleles; ++j) {
            vars[i].allele_ids[j] = bgen_str_fread(file, 4);
        }
    }

    return vars;
err:
    if (vars)
        bgen_free_partition(vars, *nvariants);
    return NULL;
}

void bgen_free_partition(struct bgen_vm* vars, uint32_t nvars)
{
    for (uint32_t i = 0; i < nvars; ++i)
        free_metadata(vars + i);
    free(vars);
}

int bgen_mf_close(struct bgen_mf* mf)
{
    if (mf) {
        if (fclose(mf->stream)) {
            bgen_perror("could not close the %s file", mf->filepath);
            return 1;
        }
        mf->stream = NULL;
        free_c(mf->filepath);
        free_c(mf->idx.poffset);
        free_c(mf);
    }
    return 0;
}

static struct bgen_mf* bgen_mf_alloc(char const* filepath)
{
    struct bgen_mf* mf = malloc(sizeof(struct bgen_mf));
    mf->filepath = strdup(filepath);
    mf->stream = NULL;
    mf->idx.poffset = NULL;
    return mf;
}

static uint32_t partition_nvariants(struct bgen_mf const* mf, uint32_t partition)
{
    uint32_t size = ceildiv_uint32(mf->idx.nvariants, mf->idx.npartitions);
    return min_uint32(size, mf->idx.nvariants - size * partition);
}
