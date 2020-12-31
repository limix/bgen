#ifndef BGEN_METAFILE_WRITE_H
#define BGEN_METAFILE_WRITE_H

#include "athr/athr.h"
#include "bgen/bstring.h"
#include "bgen/variant.h"
#include "bmath.h"
#include "bstring.h"
#include "io.h"
#include "metafile.h"
#include "report.h"
#include "variant.h"
#include <inttypes.h>
#include <string.h>

/* Write variant genotype to file and return the block size. */
static uint64_t write_variant(FILE* stream, const struct bgen_variant* variant)
{
    int64_t start = bgen_ftell(stream);

    if (fwrite(&variant->genotype_offset, sizeof(variant->genotype_offset), 1, stream) != 1) {
        bgen_perror("could not write genotype offset");
        return 0;
    }

    if (bgen_string_fwrite(variant->id, stream, 2)) {
        bgen_perror("could not write variant id");
        return 0;
    }

    if (bgen_string_fwrite(variant->rsid, stream, 2)) {
        bgen_perror("could not write variant rsid");
        return 0;
    }

    if (bgen_string_fwrite(variant->chrom, stream, 2)) {
        bgen_perror("could not write chromosome");
        return 0;
    }

    if (fwrite(&variant->position, sizeof(variant->position), 1, stream) != 1) {
        bgen_perror("could not write variant position");
        return 0;
    }

    if (fwrite(&variant->nalleles, sizeof(variant->nalleles), 1, stream) != 1) {
        bgen_perror("could not write number of alleles");
        return 0;
    }

    for (uint16_t j = 0; j < variant->nalleles; ++j) {
        if (bgen_string_fwrite(variant->allele_ids[j], stream, 4)) {
            return 0;
        }
    }

    int64_t stop = bgen_ftell(stream);
    if (start > stop)
        bgen_die("start cannot be greater than stop");

    return (uint64_t)(stop - start);
}

static int write_metafile_header(FILE* stream, uint32_t nvariants, uint32_t npartitions,
                                 uint64_t metadata_block_size)
{
    char const name[] = BGEN_METAFILE_SIGNATURE;

    if (fwrite(name, strlen(name), 1, stream) != 1) {
        bgen_perror("could not write signature");
        return 1;
    }

    if (fwrite(&nvariants, sizeof(nvariants), 1, stream) != 1) {
        bgen_perror("could not write the number of variants");
        return 1;
    }

    if (fwrite(&npartitions, sizeof(npartitions), 1, stream) != 1) {
        bgen_perror("could not write the number of partitions");
        return 1;
    }

    if (fwrite(&metadata_block_size, sizeof(metadata_block_size), 1, stream) != 1) {
        bgen_perror("could not write metadata block size");
        return 1;
    }

    return 0;
}

static uint64_t write_metafile_metadata_block(FILE* stream, uint64_t* poffset,
                                              uint32_t npartitions, uint32_t nvariants,
                                              struct bgen_file* bgen, int verbose)
{
    struct athr* at = NULL;
    if (verbose) {
        at = athr_create((long)nvariants, "Writing variants", ATHR_BAR | ATHR_ETA);
        if (at == NULL) {
            bgen_error("could not create a progress bar");
            goto err;
        }
    }

    uint64_t i = 0, j = 0;
    int      error = 0;

    int64_t ftold = bgen_ftell(stream);
    if (ftold < 0) {
        bgen_perror("could not ftell");
        goto err;
    }
    uint64_t curr_offset = (uint64_t)ftold;

    uint32_t part_size = bgen_metafile_partition_size(nvariants, npartitions);
    for (struct bgen_variant* variant = bgen_variant_begin(bgen, &error);
         variant != bgen_variant_end(bgen); variant = bgen_variant_next(bgen, &error)) {

        if (error) {
            bgen_error("could not write every variant");
            goto err;
        }

        uint64_t size = write_variant(stream, variant);

        if (size == 0)
            goto err;

        if (at)
            athr_consume(at, 1);

        /* true for the first variant of every partition */
        if (i % part_size == 0) {
            poffset[j] = curr_offset;
            ++j;
        }

        curr_offset += size;
        ++i;
        bgen_variant_destroy(variant);
    }

    if (verbose)
        athr_finish(at);

    return (curr_offset - (uint64_t)ftold);
err:
    return 0;
}

static int write_metafile_offsets_block(FILE* stream, uint32_t npartitions, uint64_t* poffset)
{
    if (fwrite(poffset, sizeof(uint64_t) * npartitions, 1, stream) != 1) {
        bgen_perror("could not write offsets block");
        return 1;
    }

    return 0;
}

#endif
