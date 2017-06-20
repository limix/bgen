#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bgen.h"
#include "layout2.h"
#include "util/mem.h"
#include "util/file.h"

#define FAIL EXIT_FAILURE
#define SUCCESS EXIT_SUCCESS

inti bgen_read_header(BGenFile *bgen)
{
    uint32_t header_length;
    uint32_t nvariants;
    uint32_t nsamples;
    uint32_t magic_number;
    uint32_t flags;

    if (bgen_read(bgen->file, &header_length, 4) == FAIL) return FAIL;

    if (bgen_read(bgen->file, &nvariants, 4) == FAIL) return FAIL;

    if (bgen_read(bgen->file, &nsamples, 4) == FAIL) return FAIL;

    if (bgen_read(bgen->file, &magic_number, 4) == FAIL) return FAIL;

    fseek(bgen->file, header_length - 20, SEEK_CUR);

    if (bgen_read(bgen->file, &flags, 4)) return FAIL;

    bgen->nvariants           = nvariants;
    bgen->nsamples            = nsamples;
    bgen->compression         = flags & 3;
    bgen->layout              = (flags & (15 << 2)) >> 2;
    bgen->sample_ids_presence = (flags & (1 << 31)) >> 31;

    return SUCCESS;
}

BGenFile* bgen_open(const byte *filepath)
{
    BGenFile *bgen = malloc(sizeof(BGenFile));

    bgen->filepath = bgen_strdup(filepath);

    bgen->file = fopen((const char *)bgen->filepath, "rb");

    if (bgen->file == NULL) {
        fprintf(stderr, "Could not open: %s\n", bgen->filepath);
        goto err;
    }

    bgen->samples_start  = -1;
    bgen->variants_start = -1;

    uint32_t offset;

    // First four bytes (offset)
    if (bgen_read(bgen->file, &offset, 4) == FAIL) goto err;

    if (bgen_read_header(bgen) == FAIL) goto err;


    if (bgen->sample_ids_presence == 0) bgen->variants_start = ftell(bgen->file);
    else {
        bgen->samples_start = ftell(bgen->file);

        if (bgen->samples_start == EOF)
        {
            perror("Could not find sample blocks.");
            goto err;
        }
    }

    if (fclose(bgen->file))
    {
        perror("Could not close the file.");
        goto err;
    }

    return bgen;

err:

    if (bgen->file) fclose(bgen->file);

    if (bgen->filepath) free(bgen->filepath);

    if (bgen != NULL) free(bgen);
    return NULL;
}

void bgen_close(BGenFile *bgen)
{
    if (bgen->filepath) free(bgen->filepath);

    if (bgen) free(bgen);
}

inti bgen_nsamples(BGenFile *bgen)
{
    return bgen->nsamples;
}

inti bgen_nvariants(BGenFile *bgen)
{
    return bgen->nvariants;
}

string* bgen_read_samples(BGenFile *bgen)
{
    if (bgen->sample_ids_presence == 0) return NULL;

    bgen->file = fopen((const char *)bgen->filepath, "rb");

    fseek(bgen->file, bgen->samples_start, SEEK_SET);

    string *sample_ids = malloc(bgen->nsamples * sizeof(string));

    uint32_t length, nsamples;
    uint16_t len;

    if (bgen_read(bgen->file, &length, 4)) goto err;

    if (bgen_read(bgen->file, &nsamples, 4)) goto err;

    inti i;

    for (i = 0; i < bgen->nsamples; ++i)
    {
        if (bgen_read(bgen->file, &len, 2)) goto err;

        string_alloc(sample_ids + i, len);

        if (bgen_read(bgen->file, sample_ids[i].str, len)) goto err;
    }

    bgen->variants_start = ftell(bgen->file);

    if (fclose(bgen->file))
    {
        perror("Could not close the file.");
        goto err;
    }

    return sample_ids;

err:

    if (bgen->file != NULL) fclose(bgen->file);

    if (sample_ids != NULL) free(sample_ids);
    return NULL;
}

void bgen_free_samples(const BGenFile *bgen, string *samples)
{
    if (bgen->sample_ids_presence == 0) return;

    if (samples == NULL) return;

    inti i;

    for (i = 0; i < bgen->nsamples; ++i)
    {
        free(samples[i].str);
    }
    free(samples);
}

inti bgen_read_variant(BGenFile *bgen, Variant *v)
{
    uint32_t nsamples, position, allele_len;
    uint16_t id_len, rsid_len, chrom_len, nalleles;

    if (bgen->layout == 1)
    {
        if (bgen_read(bgen->file, &nsamples, 4) == FAIL) return FAIL;
    }

    if (bgen_read(bgen->file, &id_len, 2) == FAIL) return FAIL;

    string_alloc(&(v->id), id_len);

    if (bgen_read(bgen->file, v->id.str, v->id.len) == FAIL) return FAIL;

    if (bgen_read(bgen->file, &rsid_len, 2) == FAIL) return FAIL;

    string_alloc(&(v->rsid), rsid_len);

    if (bgen_read(bgen->file, v->rsid.str, v->rsid.len) == FAIL) return FAIL;

    if (bgen_read(bgen->file, &chrom_len, 2) == FAIL) return FAIL;

    string_alloc(&(v->chrom), chrom_len);

    if (bgen_read(bgen->file, v->chrom.str, v->chrom.len) == FAIL) return FAIL;

    if (bgen_read(bgen->file, &position, 4) == FAIL) return FAIL;

    v->position = position;

    if (bgen->layout == 1) nalleles = 2;
    else if (bgen_read(bgen->file, &nalleles, 2) == FAIL) return FAIL;

    v->nalleles = nalleles;

    v->allele_ids = malloc(nalleles * sizeof(string));

    inti i;

    for (i = 0; i < v->nalleles; ++i)
    {
        if (bgen_read(bgen->file, &allele_len, 4) == FAIL) return FAIL;

        string_alloc(v->allele_ids + i, allele_len);

        if (bgen_read(bgen->file, v->allele_ids[i].str, allele_len) == FAIL) return FAIL;
    }

    return SUCCESS;
}

Variant* bgen_read_variants(BGenFile *bgen, VariantIndexing **index)
{
    Variant *variants = NULL;
    uint32_t length;

    *index = malloc(sizeof(VariantIndexing));

    bgen->file = fopen((const char *)bgen->filepath, "rb");

    fseek(bgen->file, bgen->variants_start, SEEK_SET);

    (*index)->filepath    = bgen_strdup(bgen->filepath);
    (*index)->compression = bgen->compression;
    (*index)->layout      = bgen->layout;
    (*index)->nsamples    = bgen->nsamples;
    (*index)->nvariants   = bgen->nvariants;

    inti nvariants = bgen->nvariants;

    variants        = malloc(nvariants * sizeof(Variant));
    (*index)->start = malloc(nvariants * sizeof(inti));

    inti i;

    for (i = 0; i < nvariants; ++i)
    {
        if (bgen_read_variant(bgen, variants + i) == FAIL) goto err;
        (*index)->start[i] = ftell(bgen->file);

        if (bgen_read(bgen->file, &length, 4) == FAIL) goto err;

        fseek(bgen->file, length, SEEK_CUR);
    }

    if (fclose(bgen->file))
    {
        perror("Could not close the file.");
        goto err;
    }

    return variants;

err:

    if (bgen->file) fclose(bgen->file);

    if (*index != NULL) free(*index);

    if (variants != NULL) free(variants);

    if ((*index)->start != NULL) free((*index)->start);
    return NULL;
}

void bgen_free_variants(const BGenFile *bgen,
                        Variant        *variants)
{
    inti i, j;

    for (i = 0; i < bgen->nvariants; ++i)
    {
        for (j = 0; j < variants[i].nalleles; ++j)
        {
            free(variants[i].allele_ids[j].str);
        }
        free(variants[i].allele_ids);
        free(variants[i].id.str);
        free(variants[i].rsid.str);
        free(variants[i].chrom.str);
    }
    free(variants);
}

void bgen_free_indexing(VariantIndexing *index)
{
    free(index->filepath);
    free(index->start);
    free(index);
}

VariantGenotype* bgen_open_variant_genotype(VariantIndexing *indexing,
                                            inti             variant_idx)
{
    indexing->file = fopen((const char *)indexing->filepath, "rb");

    if (indexing->file == NULL) {
        fprintf(stderr, "Could not open: %s\n", indexing->filepath);
        return NULL;
    }

    VariantGenotype *vg = malloc(sizeof(VariantGenotype));

    vg->variant_idx = variant_idx;
    fseek(indexing->file, indexing->start[variant_idx], SEEK_SET);

    bgen_read_variant_genotype_header_layout2(indexing, vg);

    return vg;
}

void bgen_read_variant_genotype(VariantIndexing *indexing, VariantGenotype *vg,
                                real *probabilities)
{
    bgen_read_variant_genotype_probabilities_layout2(indexing, vg, probabilities);
}

inti bgen_variant_genotype_nsamples(VariantGenotype *vg)
{
    return vg->nsamples;
}

inti bgen_variant_genotype_nalleles(VariantGenotype *vg)
{
    return vg->nalleles;
}

inti bgen_variant_genotype_ploidy(VariantGenotype *vg)
{
    return vg->ploidy;
}

inti bgen_variant_genotype_ncombs(VariantGenotype *vg)
{
    return vg->ncombs;
}

void bgen_close_variant_genotype(VariantIndexing *indexing,
                                 VariantGenotype *vg)
{
    if (indexing->file) fclose(indexing->file);

    if (vg->plo_miss != NULL) free(vg->plo_miss);

    if (vg->chunk != NULL) free(vg->chunk);

    if (vg != NULL) free(vg);
}

inti bgen_sample_ids_presence(BGenFile *bgen)
{
    return bgen->sample_ids_presence;
}
