#define BGEN_API_EXPORTS

#include "athr.h"
#include "bgen.h"
#include "file.h"
#include "io.h"
#include "mem.h"
#include "min.h"
#include "str.h"
#include "variant.h"
#include <assert.h>

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
    FILE *fp;
    struct bgen_idx idx;
};

struct bgen_mf *alloc_mf(void)
{
    struct bgen_mf *mf = dalloc(sizeof(struct bgen_mf));
    if (!mf)
        return NULL;

    mf->filepath = NULL;
    mf->fp = NULL;
    mf->idx.poffset = NULL;

    return mf;
}

inline static int ceildiv(size_t x, size_t y) { return (x + (y - 1)) / y; }

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
 */
int _next_variant(struct bgen_vm *vm, uint64_t *geno_offset, struct next_variant_ctx *c)
{
    if (c->nvariants == 0)
        goto err;

    if (next_variant(c->bgen, vm))
        goto err;

    *geno_offset = ftell(c->bgen->file);

    uint32_t length;
    if (fread_ui32(c->bgen->file, &length, 4))
        goto err;

    if (fseek(c->bgen->file, length, SEEK_CUR)) {
        perror_fmt("Could not jump to the next variant");
        goto err;
    }

    return (c->nvariants)--;
err:
    free_metadata(vm);
    return 0;
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

    if (!(mf->fp = fopen(filepath, "wb"))) {
        perror_fmt("Could not create file %s", filepath);
        goto err;
    }

    mf->filepath = strdup(filepath);

    if (fwrite1(BGEN_IDX_NAME BGEN_IDX_VER, BGEN_HDR_LEN * sizeof(char), mf->fp))
        goto err;

    if (fwrite1(&nvars, sizeof(uint32_t), mf->fp))
        goto err;

    if (fseek(mf->fp, sizeof(uint64_t), SEEK_CUR))
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
    long start = ftell(fp);

    fwrite_ui64(fp, offset, 8);
    fwrite_str(fp, &(v->id), 2);
    fwrite_str(fp, &(v->rsid), 2);
    fwrite_str(fp, &(v->chrom), 2);
    fwrite_int(fp, v->position, 4);
    fwrite_int(fp, v->nalleles, 2);

    for (size_t j = 0; j < (size_t)v->nalleles; ++j)
        fwrite_str(fp, v->allele_ids + j, 4);

    return ftell(fp) - start;
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
    uint64_t size;
    uint64_t offset;
    struct athr *at = NULL;

    mf->idx.poffset = dalloc(sizeof(uint64_t) * (mf->idx.npartitions + 1));
    if (mf->idx.poffset == NULL)
        goto err;

    if (verbose)
        at = athr_create(mf->idx.nvariants, "Writing variants", ATHR_BAR | ATHR_ETA);

    mf->idx.poffset[0] = 0;
    size_t i = 0, j = 0;
    int part_size = ceildiv(mf->idx.nvariants, mf->idx.npartitions);
    /* Write the first block. */
    while (next(&vm, &offset, args)) {

        if ((size = write_variant(mf->fp, &vm, offset)) == 0)
            goto err;

        if (verbose)
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

    fwrite_ui32(mf->fp, mf->idx.npartitions, sizeof(mf->idx.npartitions));

    /* Write the second block. */
    for (i = 0; i < mf->idx.npartitions; ++i)
        fwrite_ui64(mf->fp, mf->idx.poffset[i], sizeof(uint64_t));

    fseek(mf->fp, BGEN_HDR_LEN + sizeof(uint32_t), SEEK_SET);
    fwrite_ui64(mf->fp, mf->idx.poffset[mf->idx.npartitions], sizeof(uint64_t));

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

    struct next_variant_ctx ctx;
    ctx.bgen = bgen;
    ctx.nvariants = bgen_nvariants(bgen);

    if (fseek(bgen->file, bgen->variants_start, SEEK_SET)) {
        perror_fmt("Could not jump to the variants start");
        goto err;
    }

    if (write_metafile(mf, &_next_variant, &ctx, verbose))
        goto err;

    if (fclose_nul(mf->fp))
        goto err;
    mf->fp = NULL;

    return mf;

err:
    bgen_close_metafile(mf);
    return NULL;
}

BGEN_API struct bgen_mf *bgen_open_metafile(const char *filepath)
{
    struct bgen_mf *mf = alloc_mf();
    if (!mf) {
        error("Could not allocate resources for metafile.");
        goto err;
    }

    mf->filepath = strdup(filepath);

    if (!(mf->fp = fopen(mf->filepath, "rb"))) {
        perror_fmt("Could not open %s", mf->filepath);
        goto err;
    }

    char header[13];
    if (fread1(header, 13 * sizeof(char), mf->fp)) {
        perror_fmt("Could not fetch the metafile header");
        goto err;
    }

    if (strncmp(header, "bgen index 03", 13)) {
        error("Unrecognized bgen index version: %.13s.", header);
        goto err;
    }

    if (fread1(&(mf->idx.nvariants), sizeof(uint32_t), mf->fp)) {
        perror_fmt("Could not read the number of variants from metafile");
        goto err;
    }

    if (fread1(&(mf->idx.metasize), sizeof(uint64_t), mf->fp)) {
        perror_fmt("Could not read the metasize from metafile");
        goto err;
    }

    if (fseek(mf->fp, mf->idx.metasize, SEEK_CUR)) {
        error("Could to fseek to the number of partitions.");
        goto err;
    }

    if (fread1(&(mf->idx.npartitions), sizeof(uint32_t), mf->fp)) {
        perror_fmt("Could not read the number of partitions");
        goto err;
    }

    mf->idx.poffset = dalloc(mf->idx.npartitions * sizeof(uint64_t));

    for (size_t i = 0; i < mf->idx.npartitions; ++i) {
        if (fread1(mf->idx.poffset + i, sizeof(uint64_t), mf->fp)) {
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
        if (fclose_nul(mf->fp)) {
            perror_fmt("Could not close the %s file.", mf->filepath);
            return 1;
        }
        mf->fp = NULL;
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
    int size = mf->idx.nvariants / mf->idx.npartitions;
    return imin(size, mf->idx.nvariants - size * part);
}

BGEN_API struct bgen_vm *bgen_read_partition(struct bgen_mf *mf, int part, int *nvars)
{
    struct bgen_vm *vars = NULL;
    FILE *fp = NULL;

    if ((uint32_t)part >= mf->idx.npartitions) {
        error("The provided partition number %d is out-of-range.", part);
        goto err;
    }

    int chunk = ceildiv(mf->idx.nvariants, mf->idx.npartitions);
    *nvars = imin(chunk, mf->idx.nvariants - chunk * part);
    vars = dalloc((*nvars) * sizeof(struct bgen_vm));
    for (int i = 0; i < *nvars; ++i)
        init_metadata(vars + i);

    if ((fp = fopen(mf->filepath, "rb")) == NULL) {
        perror_fmt("Could not open bgen index file");
        goto err;
    }

    if (fseek(fp, 13 + 4 + 8, SEEK_SET)) {
        perror_fmt("Could not fseek bgen index file");
        goto err;
    }

    if (fseek(fp, mf->idx.poffset[part], SEEK_CUR)) {
        perror_fmt("Could not fseek bgen index file");
        goto err;
    }

    for (int i = 0; i < *nvars; ++i) {
        fread_long(fp, &vars[i].vaddr, 8);

        fread_str(fp, &vars[i].id, 2);
        fread_str(fp, &vars[i].rsid, 2);
        fread_str(fp, &vars[i].chrom, 2);

        fread_int(fp, &vars[i].position, 4);
        fread_int(fp, &vars[i].nalleles, 2);
        vars[i].allele_ids = dalloc(sizeof(struct bgen_str) * vars[i].nalleles);

        for (int j = 0; j < vars[i].nalleles; ++j)
            fread_str(fp, vars[i].allele_ids + j, 4);
    }

    if (fclose_nul(fp)) {
        perror_fmt("Could not close file after reading partition");
        goto err;
    }

    return vars;
err:
    if (vars != NULL)
        bgen_free_partition(vars, *nvars);
    fclose_nul(fp);
    return NULL;
}

BGEN_API void bgen_free_partition(struct bgen_vm *vars, int nvars)
{
    for (size_t i = 0; i < (size_t)nvars; ++i)
        free_metadata(vars + i);
    free(vars);
}
