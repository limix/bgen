#define BGEN_API_EXPORTS

#include "meta.h"
#include "bits.h"
#include "endian.h"
#include "file.h"
#include "io.h"
#include "mem.h"
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
    /* block size */
    uint64_t metasize;
    uint32_t npartitions;
    uint64_t *offset;
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
    mf->idx.offset = NULL;

    return mf;
}

void free_mf(struct bgen_mf *mf)
{
    if (mf) {
        mf->filepath = free_nul(mf->filepath);
        mf->fp = fclose_nul(mf->fp);
        mf->idx.offset = free_nul(mf->idx.offset);
    }
    free_nul(mf);
}

int next_variant(struct bgen_vm *vm, uint64_t *geno_offset, struct next_variant_ctx *c)
{
    if (c->nvariants == 0)
        goto err;

    if (read_next_variant(c->bgen, vm))
        goto err;

    *geno_offset = ftell(c->bgen->file);

    uint32_t length;
    if (fread_ui32(c->bgen->file, &length, 4))
        goto err;

    if (fseek(c->bgen->file, length, SEEK_CUR)) {
        perror_fmt("Could not jump to the next variant");
        goto err;
    }

    return --(c->nvariants);
err:
    return 0;
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
        perror("Could not create file ");
        goto err;
    }

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
    free_mf(mf);
    return NULL;
}

uint64_t write_variant(FILE *fp, const struct bgen_vm *v, uint64_t offset)
{
    long start = ftell(fp);

    fwrite_ui64(fp, offset, 8);
    fwrite_str(fp, &(v->id), 2);
    fwrite_str(fp, &(v->rsid), 2);
    fwrite_str(fp, &(v->chrom), 2);
    fwrite_int(fp, v->position, 4);
    fwrite_int(fp, v->nalleles, 2);

    for (size_t j = 0; j < v->nalleles; ++j)
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
    uint64_t size;
    uint64_t offset;

    mf->idx.offset = dalloc(sizeof(uint64_t) * (mf->idx.npartitions + 1));
    if (mf->idx.offset == NULL)
        goto err;

    mf->idx.offset[0] = 0;
    size_t i = 0, j = 0;
    /* Write the first block. */
    while (next(&vm, &offset, args)) {

        if ((size = write_variant(mf->fp, &vm, offset)) == 0)
            goto err;

        if (i % (mf->idx.nvariants / mf->idx.npartitions) == 0) {
            ++j;
            mf->idx.offset[j] = mf->idx.offset[j - 1];
        }

        mf->idx.offset[j] += size;
        ++i;
    }

    fwrite_ui32(mf->fp, mf->idx.npartitions, sizeof(mf->idx.npartitions));

    /* Write the first block. */
    for (i = 0; i < mf->idx.npartitions; ++i)
        fwrite_ui64(mf->fp, mf->idx.offset[i], sizeof(mf->idx.offset[i]));

    fseek(mf->fp, BGEN_HDR_LEN + sizeof(uint32_t), SEEK_SET);
    fwrite_ui64(mf->fp, mf->idx.offset[mf->idx.npartitions], 4);

    return 0;
err:
    mf->idx.offset = free_nul(mf->idx.offset);
    return 1;
}

BGEN_API struct bgen_mf *bgen_create_metafile(struct bgen_file *bgen, const char *fp,
                                              int verbose)
{
    assert(bgen);
    assert(fp);

    struct bgen_mf *mf = create_metafile(fp, bgen_nvariants(bgen), 2);
    if (!mf)
        goto err;

    struct next_variant_ctx ctx;
    ctx.bgen = bgen;
    ctx.nvariants = bgen_nvariants(bgen);

    bopen_or_leave(bgen);

    if (fseek(bgen->file, bgen->variants_start, SEEK_SET)) {
        perror_fmt("Could not jump to the variants start");
        goto err;
    }

    if (write_metafile(mf, &next_variant, &ctx, verbose))
        goto err;

    if (bgen_close_metafile(mf))
        goto err;

    fclose(bgen->file);
    return mf;
err:
    fclose_nul(bgen->file);
    bgen_close_metafile(mf);
    return NULL;
}

BGEN_API struct bgen_mf *bgen_open_metafile(const char *filepath)
{
    struct bgen_mf *mf = alloc_mf();
    if (!mf)
        goto err;

    mf->filepath = strdup(filepath);

    if (!(mf->fp = fopen(mf->filepath, "rb"))) {
        perror_fmt("Could not open %s", mf->filepath);
        goto err;
    }

    char header[13];
    if (fread1(header, 13 * sizeof(char), mf->fp))
        goto err;

    if (strncmp(header, "bgen index 03", 13)) {
        error("Unrecognized bgen index version: %.13s.", header);
        goto err;
    }

    if (fread1(&(mf->idx.nvariants), sizeof(uint32_t), mf->fp))
        goto err;

    if (fread1(&(mf->idx.metasize), sizeof(uint64_t), mf->fp))
        goto err;

    if (fseek(mf->fp, mf->idx.metasize, SEEK_CUR)) {
        error("Could to fseek to the number of partitions.");
        goto err;
    }

    if (fread1(&(mf->idx.npartitions), sizeof(uint32_t), mf->fp))
        goto err;

    mf->idx.offset = dalloc(mf->idx.npartitions * sizeof(uint64_t));

    for (size_t i = 0; i < mf->idx.npartitions; ++i) {
        if (fread1(mf->idx.offset + i, sizeof(uint64_t), mf->fp))
            goto err;
    }

    return mf;

err:
    bgen_close_metafile(mf);
    return NULL;
}

BGEN_API int bgen_close_metafile(struct bgen_mf *mf)
{
    assert(mf);

    free_nul(mf->idx.offset);
    fclose_nul(mf->fp);
    free_nul(mf->filepath);

    return 0;
}

BGEN_API int bgen_metafile_nparts(struct bgen_mf *mf)
{
    assert(mf);
    return mf->idx.npartitions;
}

BGEN_API int bgen_metafile_nvars(struct bgen_mf *mf)
{
    assert(mf);
    return mf->idx.nvariants;
}

BGEN_API int bgen_partition_nvars(struct bgen_mf *mf, int part)
{
    assert(mf);
    assert(part >= 0);

    int size = mf->idx.nvariants / mf->idx.npartitions;
    return MIN(size, mf->idx.nvariants - size * part);
}

BGEN_API struct bgen_vm *bgen_read_partition(struct bgen_mf *mf, int part, int *nvars)
{
    struct bgen_vm *vars = NULL;
    FILE *fp = NULL;

    if (part >= mf->idx.npartitions) {
        error("The provided partition number is out-of-range.");
        goto err;
    }

    int chunk = mf->idx.nvariants / mf->idx.npartitions;
    *nvars = MIN(chunk, mf->idx.nvariants - chunk * part);
    vars = dalloc(*nvars * sizeof(struct bgen_vm));
    int i, j;
    for (i = 0; i < *nvars; ++i) {
        vars[i].id.str = NULL;
        vars[i].rsid.str = NULL;
        vars[i].chrom.str = NULL;
        vars[i].allele_ids = NULL;
    }

    if ((fp = fopen(mf->filepath, "rb")) == NULL) {
        perror("Could not open bgen index file ");
        goto err;
    }

    if (fseek(fp, 13 + 4 + 8, SEEK_SET)) {
        perror("Could not fseek bgen index file ");
        goto err;
    }

    if (fseek(fp, mf->idx.offset[part], SEEK_CUR)) {
        perror("Could not fseek bgen index file ");
        goto err;
    }

    for (i = 0; i < *nvars; ++i) {
        fread_int(fp, &vars[i].vaddr, 8);

        fread_str(fp, &vars[i].id, 2);
        fread_str(fp, &vars[i].rsid, 2);
        fread_str(fp, &vars[i].chrom, 2);

        fread_int(fp, &vars[i].position, 4);
        fread_int(fp, &vars[i].nalleles, 2);
        vars[i].allele_ids = malloc(sizeof(struct bgen_str) * vars[i].nalleles);

        for (j = 0; j < vars[i].nalleles; ++j) {
            fread_str(fp, vars[i].allele_ids + j, 4);
        }
    }

    return vars;
err:
    if (vars != NULL) {
        for (i = 0; i < *nvars; ++i) {
            free_metadata(vars + i);
            /* if (vars[i].id.str != NULL) */
            /*     free(vars[i].id.str); */

            /* if (vars[i].rsid.str != NULL) */
            /*     free(vars[i].rsid.str); */

            /* if (vars[i].chrom.str != NULL) */
            /*     free(vars[i].chrom.str); */

            /* if (vars[i].allele_ids != NULL) { */
            /*     for (j = 0; j < vars[i].nalleles; ++j) { */
            /*         if (vars[i].allele_ids[j].str != NULL) */
            /*             free(vars[i].allele_ids[j].str); */
            /*     } */
            /*     free(vars[i].allele_ids); */
            /* } */
        }
        free(vars);
    }
    fclose_nul(fp);
    return NULL;
}

BGEN_API void bgen_free_partition(struct bgen_vm *vm, int nvars)
{
    for (size_t i = 0; i < nvars; ++i) {
        free_metadata(vm + i);
    }
    free(vm);
}
