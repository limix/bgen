/* This module reads and writes the bgen index version 03.
 *
 * A bgen index file is defined as follows.
 *
 * [ 13 chars : "bgen index 03"     ],
 * [ uint32_t : number of variants  ],
 * [ uint64_t : metadata block size ],
 * Metadata block
 *   [
 *     uint64_t        : variant offset in the bgen file
 *     uint16_t, str,  : variant id
 *     uint16_t, str,  : variant rsid
 *     uint16_t, str,  : variant chrom
 *     uint32_t,       : genetic position
 *     uint16_t,       : number of alleles
 *     [
 *       uint32_t, str : allele id
 *     ], ...
 *   ], ...
 * Meta-index block
 * [ uint32_t : npartitions ],
 * [ uint64_t : position of partition 0 ],
 * [ uint64_t : position of partition 1 ], ...
 */

#include "index.h"
#include "util/endian.h"
#include <stdio.h>
#include <stdlib.h>

/* node for creating metadata file */
struct bgen_cmf {
    FILE *fp;
    uint32_t npartitions;
    uint32_t nvariants;
    uint64_t *offset;
};

uint64_t bgen_create_metafile_write_var(FILE *fp, const struct bgen_var *, uint64_t);

struct bgen_cmf *bgen_create_metafile_open(const char *filepath, uint32_t nvariants,
                                           uint32_t npartitions) {

    struct bgen_cmf *c = malloc(sizeof(struct bgen_cmf));
    if (c == NULL)
        return NULL;

    c->offset = NULL;
    if (!bgen_is_lendian()) {
        fprintf(stderr, "This program does not support big-endian platforms yet.\n");
        free(c);
        return NULL;
    }

    if ((c->fp = fopen(filepath, "wb")) == NULL) {
        perror("Could not create file ");
        free(c);
        return NULL;
    }

    if (fwrite(BGEN_IDX_NAME BGEN_IDX_VER, BGEN_HDR_LEN * sizeof(char), 1, c->fp) != 1) {
        perror("Could not write the header to file ");
        free(c);
        fclose(c->fp);
        return NULL;
    }

    if (fwrite(&nvariants, sizeof(uint32_t), 1, c->fp) != 1) {
        perror("Could not write the number of variants to file ");
        free(c);
        fclose(c->fp);
        return NULL;
    }

    uint64_t metadata_size = 0;
    if (fwrite(&metadata_size, sizeof(uint64_t), 1, c->fp) != 1) {
        perror("Could not write the zeroed metadata size ");
        free(c);
        fclose(c->fp);
        return NULL;
    }

    c->npartitions = npartitions;
    c->nvariants = nvariants;

    return c;
}

int bgen_create_metafile_write_loop(struct bgen_cmf *cmf,
                                    struct bgen_var *(*next)(uint64_t *, void *),
                                    void *cb_args, int verbose) {

    struct bgen_var *v;
    uint64_t size, i, j;
    uint64_t genotype_offset;

    if ((cmf->offset = malloc(sizeof(uint64_t) * (cmf->npartitions + 1))) == NULL) {
        fprintf(stderr, "Could not allocate memory for offsets\n.");
        return 1;
    }

    cmf->offset[0] = 0;
    i = 0;
    j = 0;
    while ((v = next(&genotype_offset, cb_args)) != NULL) {

        if ((size = bgen_create_metafile_write_var(cmf->fp, v, genotype_offset)) == 0) {
            free(cmf->offset);
            free(v);
            cmf->offset = NULL;
            return 1;
        }

        if (i % (cmf->nvariants / cmf->npartitions) == 0) {
            ++j;
            cmf->offset[j] = cmf->offset[j - 1];
        }

        cmf->offset[j] += size;
        ++i;
        free(v);
    }

    fwrite(&(cmf->npartitions), sizeof(uint32_t), 1, cmf->fp);

    for (i = 0; i < cmf->npartitions; ++i) {
        fwrite(cmf->offset + i, sizeof(uint64_t), 1, cmf->fp);
    }

    fseek(cmf->fp, BGEN_HDR_LEN + sizeof(uint32_t), SEEK_SET);
    fwrite(cmf->offset + cmf->npartitions, sizeof(uint64_t), 1, cmf->fp);

    return 0;
}

uint64_t bgen_create_metafile_write_var(FILE *fp, const struct bgen_var *v,
                                        uint64_t genotype_offset) {
    size_t j;
    uint16_t u16, u32;
    uint64_t offset;

    long start = ftell(fp);

    fwrite(&genotype_offset, sizeof(uint64_t), 1, fp);

    u16 = v->id.len;
    fwrite(&u16, sizeof(uint16_t), 1, fp);
    fwrite(v->id.str, u16 * sizeof(char), 1, fp);

    u16 = v->rsid.len;
    fwrite(&u16, sizeof(uint16_t), 1, fp);
    fwrite(v->rsid.str, u16 * sizeof(char), 1, fp);

    u16 = v->chrom.len;
    fwrite(&u16, sizeof(uint16_t), 1, fp);
    fwrite(v->chrom.str, u16 * sizeof(char), 1, fp);

    u32 = v->position;
    fwrite(&u32, sizeof(uint32_t), 1, fp);

    u16 = v->nalleles;
    fwrite(&u16, sizeof(uint16_t), 1, fp);

    for (j = 0; j < (size_t)v->nalleles; ++j) {
        u32 = (v->allele_ids + j)->len;
        fwrite(&u32, sizeof(uint32_t), 1, fp);
        fwrite((v->allele_ids + j)->str, u32 * sizeof(char), 1, fp);
    }

    return ftell(fp) - start;
}

int bgen_create_metafile_close(struct bgen_cmf *cmf) {

    if (cmf == NULL)
        return 0;

    if (cmf->offset != NULL) {
        free(cmf->offset);
        cmf->offset = NULL;
    }

    if (fclose(cmf->fp)) {
        perror("Failed to close metafile ");
        free(cmf);
        return 1;
    }

    free(cmf);
    return 0;
}
