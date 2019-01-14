#define BGEN_API_EXPORTS

#include "bgen.h"
#include "file.h"
#include "io.h"
#include "mem.h"
#include "min.h"
#include "pbar.h"
#include "str.h"
#include "variant.h"
#include <assert.h>

/* context for reading the next variant */
struct next_variant_ctx
{
    struct bgen_file *bgen;
    uint32_t nvariants;
};

/* callback type for fetching the next variant */
typedef int(next_variant_t)(struct bgen_vm *, uint64_t *, struct next_variant_ctx *);

/* variant metadata index */
struct bgen_idx
{
    uint32_t nvariants;
    /* size of the variants metadata block */
    uint64_t metasize;
    uint32_t npartitions;
    /* array of partition offsets */
    uint64_t *poffset;
};

/* node for creating metadata file */
struct bgen_mf
{
    char *filepath;
    FILE *file;
    struct bgen_idx idx;
};

struct bgen_mf *alloc_mf(void)
{
    struct bgen_mf *mf = dalloc(sizeof(struct bgen_mf));
    if (!mf)
        return NULL;

    mf->filepath = NULL;
    mf->file = NULL;
    mf->idx.poffset = NULL;

    return mf;
}

inline static uint32_t ceildiv(uint32_t x, uint32_t y) { return (x + (y - 1)) / y; }

/* Fetch the variant metada and record the genotype offset.
 *
 * Parameters
 * ----------
 * vm
 *   Variant metadata.
 * geno_offset
 *   Genotype address in the bgen file.
 * c
 *   Context for the callback.
 *
 * Returns
 * -------
 * int
 *   Number of variants left to read plus one. `0` indicates the end of the list. `-1`
 *   indicates that an error has occurred.
 */
int _next_variant(struct bgen_vm *vm, uint64_t *geno_offset, struct next_variant_ctx *c)
{
    if (c->nvariants == 0)
        return 0;

    if (next_variant(c->bgen, vm))
        goto err;

    *geno_offset = LONG_TELL(c->bgen->file);

    uint32_t length;
    if (fread_ui32(c->bgen->file, &length, 4)) {
        error("Could not read the genotype block length");
        goto err;
    }

    if (LONG_SEEK(c->bgen->file, length, SEEK_CUR)) {
        perror_fmt("Could not jump to the next variant");
        goto err;
    }

    return (c->nvariants)--;
err:
    return -1;
}

int is_little_endian(void)
{
    int num = 1;

    return *(char *)&num == 1;
}

struct bgen_mf *create_metafile(const char *filepath, uint32_t nvars, uint32_t nparts)
{
    struct bgen_mf *mf = alloc_mf();
    if (!mf)
        goto err;

    if (!is_little_endian()) {
        error("This program does not support big-endian platforms yet.");
        goto err;
    }

    if (!(mf->file = fopen(filepath, "w+b"))) {
        perror_fmt("Could not create file %s", filepath);
        goto err;
    }

    mf->filepath = strdup(filepath);

    if (fwrite1(BGEN_IDX_NAME BGEN_IDX_VER, BGEN_HDR_LEN * sizeof(char), mf->file))
        goto err;

    if (fwrite1(&nvars, sizeof(uint32_t), mf->file))
        goto err;

    if (LONG_SEEK(mf->file, sizeof(uint64_t), SEEK_CUR))
        goto err;

    mf->idx.npartitions = nparts;
    mf->idx.nvariants = nvars;

    return mf;
err:
    bgen_close_metafile(mf);
    return NULL;
}

/* Write variant genotype to file and return the block size. */
uint64_t write_variant(FILE *fp, const struct bgen_vm *v, uint64_t offset)
{
    OFF_T start = LONG_TELL(fp);

    fwrite_ui64(fp, offset, 8);
    fwrite_str(fp, &(v->id), 2);
    fwrite_str(fp, &(v->rsid), 2);
    fwrite_str(fp, &(v->chrom), 2);
    fwrite_int(fp, v->position, 4);
    fwrite_int(fp, v->nalleles, 2);

    for (size_t j = 0; j < (size_t)v->nalleles; ++j)
        fwrite_str(fp, v->allele_ids + j, 4);

    return LONG_TELL(fp) - start;
}

int write_metadata_block(struct bgen_mf *mf, next_variant_t *next, void *args,
                         int verbose)
{
    struct bgen_vm vm;

    mf->idx.poffset[0] = 0;
    int part_size = ceildiv(mf->idx.nvariants, mf->idx.npartitions);

    struct athr *at = NULL;
    if (verbose) {
        if (!(at = create_athr(mf->idx.nvariants, "Writing variants")))
            goto err;
    }

    int e;
    uint64_t offset;
    for (size_t i = 0, j = 0; (e = next(&vm, &offset, args)) > 0;) {
        uint64_t size;

        if ((size = write_variant(mf->file, &vm, offset)) == 0)
            goto err;

        if (at)
            athr_consume(at, 1);

        /* true for the first variant of every partition */
        if (i % part_size == 0) {
            ++j;
            mf->idx.poffset[j] = mf->idx.poffset[j - 1];
        }

        mf->idx.poffset[j] += size;
        ++i;
        free_metadata(&vm);
    }
    if (verbose)
        athr_finish(at);

    if (e) {
        error("Could not write every variant");
        goto err;
    }

    return 0;
err:
    return 1;
}

int write_offsets_block(struct bgen_mf *mf)
{
    if (fwrite_ui32(mf->file, mf->idx.npartitions, sizeof(mf->idx.npartitions))) {
        error("Could not write the number of partitions %d", mf->idx.npartitions);
        goto err;
    }

    for (size_t i = 0; i < mf->idx.npartitions; ++i) {
        if (fwrite_ui64(mf->file, mf->idx.poffset[i], sizeof(uint64_t))) {
            error("Could not write a partition offset");
            goto err;
        }
    }

    if (LONG_SEEK(mf->file, BGEN_HDR_LEN + sizeof(uint32_t), SEEK_SET)) {
        perror_fmt("Could not fseek");
        goto err;
    }

    if (fwrite_ui64(mf->file, mf->idx.poffset[mf->idx.npartitions], sizeof(uint64_t))) {
        error("Could not write the size of the metadata block");
        goto err;
    }

    return 0;
err:
    return 1;
}

/* Write the body of the metafile.
 *
 * It consists in two blocks of data:
 *  - The block containing the variants metadata.
 *  - The block containing the offsets to the partitions of variants metadata.
 */
int write_metafile(struct bgen_mf *mf, next_variant_t *next, void *args, int verbose)
{
    struct bgen_vm vm;
    init_metadata(&vm);

    mf->idx.poffset = dalloc(sizeof(uint64_t) * (mf->idx.npartitions + 1));
    if (mf->idx.poffset == NULL)
        goto err;

    if (write_metadata_block(mf, next, args, verbose))
        goto err;

    if (write_offsets_block(mf))
        goto err;

    return 0;
err:
    mf->idx.poffset = free_nul(mf->idx.poffset);
    return 1;
}

BGEN_API struct bgen_mf *bgen_create_metafile(struct bgen_file *bgen, const char *fp,
                                              int nparts, int verbose)
{
    struct bgen_mf *mf = create_metafile(fp, bgen_nvariants(bgen), nparts);
    if (!mf)
        goto err;

    if (LONG_SEEK(bgen->file, bgen->variants_start, SEEK_SET)) {
        perror_fmt("Could not jump to the variants start");
        goto err;
    }

    struct next_variant_ctx ctx = {bgen, bgen_nvariants(bgen)};
    if (write_metafile(mf, &_next_variant, &ctx, verbose)) {
        error("Could not write metafile");
        goto err;
    }

    if (fflush(mf->file)) {
        perror_fmt("Could not fflush");
        goto err;
    }

    return mf;
err:
    bgen_close_metafile(mf);
    return NULL;
}

BGEN_API struct bgen_mf *bgen_open_metafile(const char *filepath)
{
    struct bgen_mf *mf = alloc_mf();
    if (!mf) {
        error("Could not allocate resources for metafile");
        goto err;
    }

    mf->filepath = strdup(filepath);

    if (!(mf->file = fopen(mf->filepath, "rb"))) {
        perror_fmt("Could not open %s", mf->filepath);
        goto err;
    }

    char header[13];
    if (fread1(header, 13 * sizeof(char), mf->file)) {
        perror_fmt("Could not fetch the metafile header");
        goto err;
    }

    if (strncmp(header, "bgen index 03", 13)) {
        error("Unrecognized bgen index version: %.13s", header);
        goto err;
    }

    if (fread1(&(mf->idx.nvariants), sizeof(uint32_t), mf->file)) {
        perror_fmt("Could not read the number of variants from metafile");
        goto err;
    }

    if (fread1(&(mf->idx.metasize), sizeof(uint64_t), mf->file)) {
        perror_fmt("Could not read the metasize from metafile");
        goto err;
    }

    if (LONG_SEEK(mf->file, mf->idx.metasize, SEEK_CUR)) {
        error("Could to fseek to the number of partitions");
        goto err;
    }

    if (fread1(&(mf->idx.npartitions), sizeof(uint32_t), mf->file)) {
        perror_fmt("Could not read the number of partitions");
        goto err;
    }

    mf->idx.poffset = dalloc(mf->idx.npartitions * sizeof(uint64_t));

    for (size_t i = 0; i < mf->idx.npartitions; ++i) {
        if (fread1(mf->idx.poffset + i, sizeof(uint64_t), mf->file)) {
            perror_fmt("Could not read partition offsets");
            goto err;
        }
    }

    return mf;
err:
    bgen_close_metafile(mf);
    return NULL;
}

BGEN_API int bgen_close_metafile(struct bgen_mf *mf)
{
    if (mf) {
        if (fclose_nul(mf->file)) {
            perror_fmt("Could not close the %s file", mf->filepath);
            return 1;
        }
        mf->file = NULL;
        free_nul(mf->filepath);
        free_nul(mf->idx.poffset);
        free_nul(mf);
    }
    return 0;
}

BGEN_API int bgen_metafile_nparts(struct bgen_mf *mf) { return mf->idx.npartitions; }

BGEN_API int bgen_metafile_nvars(struct bgen_mf *mf) { return mf->idx.nvariants; }

BGEN_API int bgen_partition_nvars(struct bgen_mf *mf, int part)
{
    if (part < 0) {
        error("Invalid partition number: %d", part);
        return -1;
    }
    int size = ceildiv(mf->idx.nvariants, mf->idx.npartitions);
    return imin(size, mf->idx.nvariants - size * part);
}

BGEN_API struct bgen_vm *bgen_read_partition(struct bgen_mf *mf, int part, int *nvars)
{
    struct bgen_vm *vars = NULL;
    FILE *file = mf->file;

    if ((uint32_t)part >= mf->idx.npartitions) {
        error("The provided partition number %d is out-of-range", part);
        goto err;
    }

    *nvars = bgen_partition_nvars(mf, part);
    vars = dalloc((*nvars) * sizeof(struct bgen_vm));
    for (int i = 0; i < *nvars; ++i)
        init_metadata(vars + i);

    /* if ((file = fopen(mf->filepath, "rb")) == NULL) { */
    /*     perror_fmt("Could not open bgen index file"); */
    /*     goto err; */
    /* } */

    if (LONG_SEEK(file, 13 + 4 + 8, SEEK_SET)) {
        perror_fmt("Could not fseek bgen index file");
        goto err;
    }

    if (LONG_SEEK(file, mf->idx.poffset[part], SEEK_CUR)) {
        perror_fmt("Could not fseek bgen index file");
        goto err;
    }

    for (int i = 0; i < *nvars; ++i) {
        fread_long(file, &vars[i].vaddr, 8);

        fread_str(file, &vars[i].id, 2);
        fread_str(file, &vars[i].rsid, 2);
        fread_str(file, &vars[i].chrom, 2);

        fread_int(file, &vars[i].position, 4);
        fread_int(file, &vars[i].nalleles, 2);
        vars[i].allele_ids = dalloc(sizeof(struct bgen_str) * vars[i].nalleles);

        for (int j = 0; j < vars[i].nalleles; ++j)
            fread_str(file, vars[i].allele_ids + j, 4);
    }

    return vars;
err:
    if (vars)
        bgen_free_partition(vars, *nvars);
    /* fclose_nul(file); */
    return NULL;
}

BGEN_API void bgen_free_partition(struct bgen_vm *vars, int nvars)
{
    for (size_t i = 0; i < (size_t)nvars; ++i)
        free_metadata(vars + i);
    free(vars);
}
