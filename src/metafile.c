#include "bgen/bgen.h"
#include "bmath.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "mem.h"
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

static struct bgen_mf* bgen_mf_alloc(char const* filepath);
static uint32_t        partition_nvariants(struct bgen_mf const* mf, uint32_t partition);
static int _next_variant(struct bgen_vm* vm, uint64_t* geno_offset, struct bgen_file* bgen,
                         uint32_t* nvariants);
static uint64_t write_variant(FILE* fp, const struct bgen_vm* v, uint64_t offset);
static int      write_metafile_header(FILE* stream);
static int      write_metafile_nvariants(FILE* stream, uint32_t nvariants);
static int      write_metafile_metadata_block(struct bgen_mf* mf, struct bgen_file* bgen,
                                              int verbose);
static int write_metafile_offsets_block(FILE* stream, uint32_t npartitions, uint64_t* poffset);

struct bgen_mf* bgen_metafile_create(struct bgen_file* bgen, char const* filepath,
                                     unsigned npartitions, int verbose)
{
    struct bgen_mf* mf = bgen_mf_alloc(filepath);
    mf->idx.npartitions = npartitions;

    if (!(mf->stream = fopen(filepath, "w+b"))) {
        bgen_perror("could not create file %s", filepath);
        goto err;
    }

    if (write_metafile_header(mf->stream))
        goto err;

    mf->idx.nvariants = bgen_file_nvariants(bgen);
    if (write_metafile_nvariants(mf->stream, mf->idx.nvariants))
        goto err;

    if (LONG_SEEK(mf->stream, sizeof(uint64_t), SEEK_CUR))
        goto err;

    if (bgen_file_seek_variants_start(bgen))
        goto err;

    /* struct next_variant_ctx ctx = {bgen, bgen_file_nvariants(bgen)}; */

    mf->idx.poffset = malloc(sizeof(uint64_t) * (mf->idx.npartitions + 1));

    if (write_metafile_metadata_block(mf, bgen, verbose))
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

unsigned bgen_metafile_npartitions(struct bgen_mf const* mf) { return mf->idx.npartitions; }

unsigned bgen_metafile_nvariants(struct bgen_mf const* mf) { return mf->idx.nvariants; }

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
static int _next_variant(struct bgen_vm* vm, uint64_t* geno_offset, struct bgen_file* bgen,
                         uint32_t* nvariants)
{
    if (*nvariants == 0)
        return 0;

    if (next_variant(bgen, vm))
        goto err;

    *geno_offset = LONG_TELL(bgen_file_stream(bgen));

    uint32_t length;
    if (fread_ui32(bgen_file_stream(bgen), &length, 4)) {
        bgen_error("Could not read the genotype block length");
        goto err;
    }

    if (LONG_SEEK(bgen_file_stream(bgen), length, SEEK_CUR)) {
        bgen_perror("Could not jump to the next variant");
        goto err;
    }

    return (*nvariants)--;
err:
    return -1;
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

    fwrite_ui32(fp, v->position, 4);
    fwrite_ui16(fp, v->nalleles, 2);

    for (size_t j = 0; j < (size_t)v->nalleles; ++j) {
        if (bgen_str_fwrite(v->allele_ids[j], fp, 4))
            return 0;
    }

    OFF_T stop = LONG_TELL(fp);
    if (start > stop)
        bgen_die("start cannot be greater than stop");
    return (uint64_t)(stop - start);
}

static int write_metafile_metadata_block(struct bgen_mf* mf, struct bgen_file* bgen,
                                         int verbose)
{
    struct bgen_vm vm;

    mf->idx.poffset[0] = 0;
    uint32_t part_size = ceildiv_uint32(mf->idx.nvariants, mf->idx.npartitions);

    struct athr* at = NULL;
    if (verbose) {
        if (!(at = create_athr(mf->idx.nvariants, "Writing variants")))
            goto err;
    }

    int      e;
    uint64_t offset;
    uint32_t nvariants = bgen_file_nvariants(bgen);
    for (size_t i = 0, j = 0; (e = _next_variant(&vm, &offset, bgen, &nvariants)) > 0;) {
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

static int write_metafile_offsets_block(FILE* stream, uint32_t npartitions, uint64_t* poffset)
{
    if (fwrite_ui32(stream, npartitions, sizeof(npartitions))) {
        bgen_error("could not write the number of partitions %d", npartitions);
        goto err;
    }

    for (size_t i = 0; i < npartitions; ++i) {
        if (fwrite_ui64(stream, poffset[i], sizeof(uint64_t))) {
            bgen_error("could not write a partition offset");
            goto err;
        }
    }

    if (LONG_SEEK(stream, BGEN_METAFILE_HDR_LENGTH + sizeof(uint32_t), SEEK_SET)) {
        bgen_perror("could not fseek");
        goto err;
    }

    if (fwrite_ui64(stream, poffset[npartitions], sizeof(uint64_t))) {
        bgen_error("could not write the size of the metadata block");
        goto err;
    }

    return 0;
err:
    return 1;
}

static int write_metafile_header(FILE* stream)
{
    char const header[BGEN_METAFILE_HDR_LENGTH] = {
        BGEN_METAFILE_HDR_NAME BGEN_METAFILE_HDR_VERSION};

    if (fwrite(header, BGEN_METAFILE_HDR_LENGTH, 1, stream) != 1) {
        bgen_perror("could not write header to file");
        return 1;
    }
    return 0;
}

static int write_metafile_nvariants(FILE* stream, uint32_t nvariants)
{
    if (fwrite(&nvariants, sizeof(nvariants), 1, stream) != 1) {
        bgen_perror("could not write the number of variants to file");
        return 1;
    }
    return 0;
}
