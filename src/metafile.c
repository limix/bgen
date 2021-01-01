#include "bgen/metafile.h"
#include "bgen/file.h"
#include "bgen/variant.h"
#include "bmath.h"
#include "bstring.h"
#include "file.h"
#include "free.h"
#include "io.h"
#include "mem.h"
#include "metafile.h"
#include "metafile_write.h"
#include "partition.h"
#include "report.h"
#include <string.h>

static struct bgen_metafile* metafile_alloc(char const* filepath);
static uint32_t              compute_nvariants(uint32_t nvariants, uint32_t npartitions,
                                               uint32_t partition);

struct bgen_metafile* bgen_metafile_create(struct bgen_file* bgen_file, char const* filepath,
                                           uint32_t npartitions, int verbose)
{
    struct bgen_metafile* metafile = metafile_alloc(filepath);
    metafile->npartitions = npartitions;
    metafile->nvariants = bgen_file_nvariants(bgen_file);

    if (!(metafile->stream = fopen(filepath, "w+b"))) {
        bgen_perror("could not create file %s", filepath);
        goto err;
    }

    /* seek to metadata block */
    int64_t block_seek = (int64_t)BGEN_METAFILE_HEADER_SIZE;
    block_seek += (int64_t)(sizeof(uint64_t) * npartitions);
    if (bgen_fseek(metafile->stream, block_seek, SEEK_SET))
        goto err;

    metafile->partition_offset = malloc(sizeof(uint64_t) * npartitions);

    uint64_t metadata_block_size =
        write_metafile_metadata_block(metafile->stream, metafile->partition_offset,
                                      npartitions, metafile->nvariants, bgen_file, verbose);

    if (metadata_block_size == 0)
        goto err;

    metafile->metadata_block_size = metadata_block_size;
    rewind(metafile->stream);

    if (write_metafile_header(metafile->stream, metafile->nvariants, npartitions,
                              metafile->metadata_block_size))
        goto err;

    if (write_metafile_offsets_block(metafile->stream, npartitions,
                                     metafile->partition_offset))
        goto err;

    if (fflush(metafile->stream)) {
        bgen_perror("could not fflush metafile");
        goto err;
    }

    return metafile;

err:
    bgen_metafile_close(metafile);
    return NULL;
}

struct bgen_metafile* bgen_metafile_open(char const* filepath)
{
    struct bgen_metafile* metafile = metafile_alloc(filepath);

    if (!(metafile->stream = fopen(metafile->filepath, "rb"))) {
        bgen_perror("could not open %s", metafile->filepath);
        goto err;
    }

    char header[] = BGEN_METAFILE_SIGNATURE;

    if (fread(header, strlen(BGEN_METAFILE_SIGNATURE), 1, metafile->stream) < 1) {
        bgen_perror_eof(metafile->stream, "could not fetch the metafile header");
        goto err;
    }

    if (strncmp(header, BGEN_METAFILE_SIGNATURE, strlen(BGEN_METAFILE_SIGNATURE))) {
        bgen_error("unrecognized bgen index version: %.*s",
                   (int)strlen(BGEN_METAFILE_SIGNATURE), header);
        goto err;
    }

    if (fread(&(metafile->nvariants), sizeof(uint32_t), 1, metafile->stream) < 1) {
        bgen_perror_eof(metafile->stream,
                        "could not read the number of variants from metafile");
        goto err;
    }

    if (fread(&(metafile->npartitions), sizeof(uint32_t), 1, metafile->stream) < 1) {
        bgen_perror_eof(metafile->stream, "could not read the number of partitions");
        goto err;
    }

    if (fread(&(metafile->metadata_block_size), sizeof(uint64_t), 1, metafile->stream) < 1) {
        bgen_perror_eof(metafile->stream, "could not read the metadata block size");
        goto err;
    }

    metafile->partition_offset = malloc(metafile->npartitions * sizeof(uint64_t));

    for (uint32_t i = 0; i < metafile->npartitions; ++i) {
        uint64_t* ptr = metafile->partition_offset + i;
        if (fread(ptr, sizeof(uint64_t), 1, metafile->stream) < 1) {
            bgen_perror_eof(metafile->stream, "Could not read partition offsets");
            goto err;
        }
    }

    return metafile;

err:
    bgen_metafile_close(metafile);
    return NULL;
}

uint32_t bgen_metafile_npartitions(struct bgen_metafile const* metafile)
{
    return metafile->npartitions;
}

uint32_t bgen_metafile_nvariants(struct bgen_metafile const* metafile)
{
    return metafile->nvariants;
}

struct bgen_partition const* bgen_metafile_read_partition(struct bgen_metafile const* metafile,
                                                          uint32_t partition)
{
    FILE* stream = metafile->stream;
    char* block = NULL;

    if (partition >= metafile->npartitions) {
        bgen_error("the provided partition number %" PRIu32 " is out-of-range", partition);
        return NULL;
    }

    uint32_t const nvariants =
        compute_nvariants(metafile->nvariants, metafile->npartitions, partition);

    struct bgen_partition* part = bgen_partition_create(nvariants);

    if (metafile->partition_offset[partition] > INT64_MAX) {
        bgen_error("`partition_offset` overflow");
        goto err;
    }

    if (bgen_fseek(stream, (int64_t)metafile->partition_offset[partition], SEEK_SET)) {
        bgen_perror("could not fseek partition");
        goto err;
    }

    uint64_t        block_size = 0;
    uint64_t const* poffset = metafile->partition_offset;
    if (partition == metafile->npartitions - 1)
        block_size = metafile->metadata_block_size - (poffset[partition] - poffset[0]);
    else
        block_size = poffset[partition + 1] - poffset[partition];

    block = malloc(block_size);
    if (fread(block, block_size, 1, stream) < 1) {
        bgen_perror_eof(stream, "could not read partition");
        goto err;
    }

    char const* block_ptr = block;
    for (uint32_t i = 0; i < nvariants; ++i) {
        struct bgen_variant* v = bgen_variant_create();

        bgen_memfread(&v->genotype_offset, &block_ptr, sizeof(v->genotype_offset));
        v->id = bgen_string_memfread(&block_ptr, 2);
        v->rsid = bgen_string_memfread(&block_ptr, 2);
        v->chrom = bgen_string_memfread(&block_ptr, 2);
        bgen_memfread(&v->position, &block_ptr, sizeof(v->position));
        bgen_memfread(&v->nalleles, &block_ptr, sizeof(v->nalleles));

        bgen_variant_create_alleles(v, v->nalleles);

        for (uint16_t j = 0; j < v->nalleles; ++j)
            v->allele_ids[j] = bgen_string_memfread(&block_ptr, 4);

        bgen_partition_set(part, i, v);
    }

    bgen_free(block);
    return part;

err:
    bgen_partition_destroy(part);
    bgen_free(block);
    return NULL;
}

int bgen_metafile_close(struct bgen_metafile const* metafile)
{
    bgen_free(metafile->filepath);
    bgen_free(metafile->partition_offset);

    if (metafile->stream && fclose(metafile->stream)) {
        bgen_perror("could not close %s", metafile->filepath);
        bgen_free(metafile);
        return 1;
    }

    bgen_free(metafile);
    return 0;
}

uint32_t bgen_metafile_partition_size(uint32_t nvariants, uint32_t npartitions)
{
    return ceildiv_uint32(nvariants, npartitions);
}

static struct bgen_metafile* metafile_alloc(char const* filepath)
{
    struct bgen_metafile* metafile = malloc(sizeof(struct bgen_metafile));
    metafile->filepath = strdup(filepath);
    metafile->stream = NULL;
    metafile->partition_offset = NULL;
    return metafile;
}

static uint32_t compute_nvariants(uint32_t nvariants, uint32_t npartitions, uint32_t partition)
{
    uint32_t size = bgen_metafile_partition_size(nvariants, npartitions);
    return min_uint32(size, nvariants - size * partition);
}
