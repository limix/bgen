#include "bgen/bgen.h"
#include "endian.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "mem.h"
#include "min.h"
#include "pbar.h"
#include "report.h"
#include "str.h"
#include "variant.h"
#include <assert.h>

/* context for reading the next variant */
struct next_variant_ctx
{
    struct bgen_file* bgen;
    uint32_t          nvariants;
};

/* callback type for fetching the next variant */
typedef int(next_variant_t)(struct bgen_vm*, uint64_t*, struct next_variant_ctx*);

/* variant metadata index */
struct bgen_idx
{
    uint32_t nvariants;
    /* size of the variants metadata block */
    uint64_t metasize;
    uint32_t npartitions;
    /* array of partition offsets */
    uint64_t* poffset;
};

/* node for creating metadata file */
struct bgen_mf
{
    char*           filepath;
    FILE*           stream;
    struct bgen_idx idx;
};

inline static uint32_t ceildiv(uint32_t x, uint32_t y) { return (x + (y - 1)) / y; }

static struct bgen_mf* bgen_mf_alloc(void);
static int             bgen_partition_nvars(struct bgen_mf const* mf, int part);
static int             _next_variant(struct bgen_vm* vm, uint64_t* geno_offset,
                                     struct next_variant_ctx* c);
static struct bgen_mf* create_metafile(const char* filepath, uint32_t nvariants,
                                       uint32_t npartitions);
static uint64_t        write_variant(FILE* fp, const struct bgen_vm* v, uint64_t offset);
static int write_metafile_header(struct bgen_mf* mf);
static int write_metadata_block(struct bgen_mf* mf, next_variant_t* next, void* args,
                                int verbose);
static int write_offsets_block(struct bgen_mf* mf);
static int write_metafile(struct bgen_mf* mf, next_variant_t* next, void* args, int verbose);

struct bgen_mf* bgen_metafile_create(struct bgen_file* bgen, char const* filepath,
                                     unsigned npartitions, int verbose)
{
    struct bgen_mf* mf = create_metafile(filepath, bgen_file_nvariants(bgen), npartitions);
    if (!mf)
        goto err;

    if (bgen_file_seek_variants_start(bgen))
        goto err;

    struct next_variant_ctx ctx = {bgen, bgen_file_nvariants(bgen)};
    if (write_metafile(mf, &_next_variant, &ctx, verbose)) {
        bgen_error("Could not write metafile");
        goto err;
    }

    if (fflush(mf->stream)) {
        bgen_perror("Could not fflush");
        goto err;
    }

    return mf;
err:
    bgen_mf_close(mf);
    return NULL;
}

struct bgen_mf* bgen_open_metafile(const char* filepath)
{
    struct bgen_mf* mf = bgen_mf_alloc();
    if (!mf) {
        bgen_error("Could not allocate resources for metafile");
        goto err;
    }

    mf->filepath = strdup(filepath);

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

unsigned bgen_metafile_npartitions(struct bgen_mf const* mf) { return mf->idx.npartitions; }

unsigned bgen_metafile_nvariants(struct bgen_mf const* mf) { return mf->idx.nvariants; }

struct bgen_vm* bgen_read_partition(struct bgen_mf const* mf, int part, int* nvars)
{
    struct bgen_vm* vars = NULL;
    FILE*           file = mf->stream;

    if ((uint32_t)part >= mf->idx.npartitions) {
        bgen_error("The provided partition number %d is out-of-range", part);
        goto err;
    }

    *nvars = bgen_partition_nvars(mf, part);
    vars = malloc((*nvars) * sizeof(struct bgen_vm));
    for (int i = 0; i < *nvars; ++i)
        init_metadata(vars + i);

    if (LONG_SEEK(file, 13 + 4 + 8, SEEK_SET)) {
        bgen_perror("Could not fseek bgen index file");
        goto err;
    }

    if (LONG_SEEK(file, mf->idx.poffset[part], SEEK_CUR)) {
        bgen_perror("Could not fseek bgen index file");
        goto err;
    }

    for (int i = 0; i < *nvars; ++i) {
        fread_long(file, &vars[i].vaddr, 8);

        vars[i].id = bgen_str_fread(file, 2);
        vars[i].rsid = bgen_str_fread(file, 2);
        vars[i].chrom = bgen_str_fread(file, 2);

        fread_int(file, &vars[i].position, 4);
        fread_int(file, &vars[i].nalleles, 2);
        vars[i].allele_ids = malloc(sizeof(struct bgen_str*) * vars[i].nalleles);

        for (int j = 0; j < vars[i].nalleles; ++j) {
            vars[i].allele_ids[j] = bgen_str_fread(file, 4);
        }
    }

    return vars;
err:
    if (vars)
        bgen_free_partition(vars, *nvars);
    return NULL;
}

void bgen_free_partition(struct bgen_vm* vars, int nvars)
{
    for (size_t i = 0; i < (size_t)nvars; ++i)
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

static struct bgen_mf* bgen_mf_alloc(void)
{
    struct bgen_mf* mf = malloc(sizeof(struct bgen_mf));
    mf->filepath = NULL;
    mf->stream = NULL;
    mf->idx.poffset = NULL;
    return mf;
}

static int bgen_partition_nvars(struct bgen_mf const* mf, int part)
{
    if (part < 0) {
        bgen_error("Invalid partition number: %d", part);
        return -1;
    }
    int size = ceildiv(mf->idx.nvariants, mf->idx.npartitions);
    return imin(size, mf->idx.nvariants - size * part);
}

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
static int _next_variant(struct bgen_vm* vm, uint64_t* geno_offset, struct next_variant_ctx* c)
{
    if (c->nvariants == 0)
        return 0;

    if (next_variant(c->bgen, vm))
        goto err;

    *geno_offset = LONG_TELL(bgen_file_stream(c->bgen));

    uint32_t length;
    if (fread_ui32(bgen_file_stream(c->bgen), &length, 4)) {
        bgen_error("Could not read the genotype block length");
        goto err;
    }

    if (LONG_SEEK(bgen_file_stream(c->bgen), length, SEEK_CUR)) {
        bgen_perror("Could not jump to the next variant");
        goto err;
    }

    return (c->nvariants)--;
err:
    return -1;
}

struct bgen_mf* create_metafile(const char* filepath, uint32_t nvariants, uint32_t npartitions)
{
    struct bgen_mf* mf = bgen_mf_alloc();
    if (!mf)
        goto err;

    if (!is_little_endian()) {
        bgen_error("this program does not support big-endian platforms yet.");
        goto err;
    }

    if (!(mf->stream = fopen(filepath, "w+b"))) {
        bgen_perror("could not create file %s", filepath);
        goto err;
    }

    mf->filepath = strdup(filepath);

    if (write_metafile_header(mf))
        goto err;

    if (fwrite(&nvariants, sizeof(nvariants), 1, mf->stream) != 1) {
        bgen_perror("could not write the number of variants to file");
        goto err;
    }

    if (LONG_SEEK(mf->stream, sizeof(uint64_t), SEEK_CUR))
        goto err;

    mf->idx.npartitions = npartitions;
    mf->idx.nvariants = nvariants;

    return mf;
err:
    bgen_mf_close(mf);
    return NULL;
}

/* Write variant genotype to file and return the block size. */
static uint64_t write_variant(FILE* fp, const struct bgen_vm* v, uint64_t offset)
{
    OFF_T start = LONG_TELL(fp);

    fwrite_ui64(fp, offset, 8);
    if (bgen_str_fwrite(v->id, fp, 2))
        return 0;

    if (bgen_str_fwrite(v->rsid, fp, 2))
        return 0;

    if (bgen_str_fwrite(v->chrom, fp, 2))
        return 0;

    fwrite_int(fp, v->position, 4);
    fwrite_int(fp, v->nalleles, 2);

    for (size_t j = 0; j < (size_t)v->nalleles; ++j) {
        if (bgen_str_fwrite(v->allele_ids[j], fp, 4))
            return 0;
    }

    return LONG_TELL(fp) - start;
}

static int write_metadata_block(struct bgen_mf* mf, next_variant_t* next, void* args,
                                int verbose)
{
    struct bgen_vm vm;

    mf->idx.poffset[0] = 0;
    int part_size = ceildiv(mf->idx.nvariants, mf->idx.npartitions);

    struct athr* at = NULL;
    if (verbose) {
        if (!(at = create_athr(mf->idx.nvariants, "Writing variants")))
            goto err;
    }

    int      e;
    uint64_t offset;
    for (size_t i = 0, j = 0; (e = next(&vm, &offset, args)) > 0;) {
        uint64_t size;

        if ((size = write_variant(mf->stream, &vm, offset)) == 0)
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
        bgen_error("Could not write every variant");
        goto err;
    }

    return 0;
err:
    return 1;
}

static int write_offsets_block(struct bgen_mf* mf)
{
    if (fwrite_ui32(mf->stream, mf->idx.npartitions, sizeof(mf->idx.npartitions))) {
        bgen_error("Could not write the number of partitions %d", mf->idx.npartitions);
        goto err;
    }

    for (size_t i = 0; i < mf->idx.npartitions; ++i) {
        if (fwrite_ui64(mf->stream, mf->idx.poffset[i], sizeof(uint64_t))) {
            bgen_error("Could not write a partition offset");
            goto err;
        }
    }

    if (LONG_SEEK(mf->stream, BGEN_METAFILE_HDR_LENGTH + sizeof(uint32_t), SEEK_SET)) {
        bgen_perror("Could not fseek");
        goto err;
    }

    if (fwrite_ui64(mf->stream, mf->idx.poffset[mf->idx.npartitions], sizeof(uint64_t))) {
        bgen_error("Could not write the size of the metadata block");
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
static int write_metafile(struct bgen_mf* mf, next_variant_t* next, void* args, int verbose)
{
    struct bgen_vm vm;
    init_metadata(&vm);

    mf->idx.poffset = malloc(sizeof(uint64_t) * (mf->idx.npartitions + 1));
    if (mf->idx.poffset == NULL)
        goto err;

    if (write_metadata_block(mf, next, args, verbose))
        goto err;

    if (write_offsets_block(mf))
        goto err;

    return 0;
err:
    free_c(mf->idx.poffset);
    mf->idx.poffset = NULL;
    return 1;
}

static int write_metafile_header(struct bgen_mf* mf)
{
    char const header[BGEN_METAFILE_HDR_LENGTH] = {
        BGEN_METAFILE_HDR_NAME BGEN_METAFILE_HDR_VERSION};

    if (fwrite(header, BGEN_METAFILE_HDR_LENGTH, 1, mf->stream) != 1) {
        bgen_perror("could not write header to file");
        return 1;
    }
    return 0;
}
