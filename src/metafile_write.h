#ifndef BGEN_METAFILE_WRITE_H
#define BGEN_METAFILE_WRITE_H

#include "bmath.h"
#include "io.h"
#include "metafile.h"
#include "pbar.h"
#include "report.h"
#include "str.h"
#include "variant.h"
#include "variant_metadata.h"
#include <inttypes.h>

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
static int _next_variant(struct bgen_vm* vm, uint64_t* geno_offset, struct bgen_file* bgen,
                         uint32_t* nvariants)
{
    if (*nvariants == 0)
        return 1;

    if (next_variant(bgen, vm))
        return -1;

    *geno_offset = LONG_TELL(bgen_file_stream(bgen));

    uint32_t length;
    if (fread_ui32(bgen_file_stream(bgen), &length, 4)) {
        bgen_error("could not read the genotype block length");
        return -1;
    }

    if (LONG_SEEK(bgen_file_stream(bgen), length, SEEK_CUR)) {
        bgen_perror("could not jump to the next variant");
        return -1;
    }

    if ((*nvariants)-- == 0)
        return 1;

    return 0;
}

/* Write variant genotype to file and return the block size. */
static uint64_t write_variant(FILE* stream, const struct bgen_vm* vm)
{
    OFF_T start = LONG_TELL(stream);

    fwrite_ui64(stream, vm->genotype_offset, 8);
    if (bgen_str_fwrite(vm->id, stream, 2))
        return 0;

    if (bgen_str_fwrite(vm->rsid, stream, 2))
        return 0;

    if (bgen_str_fwrite(vm->chrom, stream, 2))
        return 0;

    fwrite_ui32(stream, vm->position, 4);
    fwrite_ui16(stream, vm->nalleles, 2);

    for (size_t j = 0; j < (size_t)vm->nalleles; ++j) {
        if (bgen_str_fwrite(vm->allele_ids[j], stream, 4))
            return 0;
    }

    OFF_T stop = LONG_TELL(stream);
    if (start > stop)
        bgen_die("start cannot be greater than stop");
    return (uint64_t)(stop - start);
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
static int write_metafile_metadata_block(FILE* stream, uint64_t* poffset, uint32_t npartitions,
                                         uint32_t nvariants, struct bgen_file* bgen,
                                         int verbose)
{
    poffset[0] = 0;
    uint32_t part_size = ceildiv_uint32(nvariants, npartitions);

    struct athr* at = NULL;
    if (verbose) {
        if (!(at = create_athr(nvariants, "Writing variants")))
            goto err;
    }

#if 0
    uint64_t offset;
    size_t   i = 0, j = 0;
    int      end = 0;
    while (!(end = _next_variant(&vm, &offset, bgen, &nvariants))) {
        uint64_t size;

        if ((size = write_variant(stream, &vm, offset)) == 0)
            goto err;

        if (at)
            athr_consume(at, 1);

        /* true for the first variant of every partition */
        if (i % part_size == 0) {
            ++j;
            poffset[j] = poffset[j - 1];
        }

        poffset[j] += size;
        ++i;
        free_metadata(&vm);
    }
#endif

    size_t i = 0, j = 0;
    int    end = 0;
    int    error = 0;
    for (struct bgen_vm* vm = bgen_variant_metadata_begin(bgen, &error);
         vm != bgen_variant_metadata_end(bgen);
         vm = bgen_variant_metadata_next(bgen, &error)) {

        if (error)
            goto err;

        uint64_t size = write_variant(stream, vm);

        if (size == 0)
            goto err;

        if (at)
            athr_consume(at, 1);

        /* true for the first variant of every partition */
        if (i % part_size == 0) {
            ++j;
            poffset[j] = poffset[j - 1];
        }

        poffset[j] += size;
        ++i;
        bgen_variant_metadata_destroy(vm);
    }

    if (verbose)
        athr_finish(at);

    if (end == -1) {
        bgen_error("could not write every variant");
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

#endif
