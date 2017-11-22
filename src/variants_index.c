#include "bgen/bgen.h"
#include "bgen/number.h"
#include "omem/omem.h"
#include "tpl/tpl.h"
#include "util/buffer.h"
#include "util/file.h"
#include "util/mem.h"
#include "util/tpl.h"
#include "variants_index.h"
#include "zip/zstd_wrapper.h"

typedef struct tpl_string {
    uint16_t len;
    byte *str;
} tpl_string;

struct TPLVar {
    tpl_string id;
    tpl_string rsid;
    tpl_string chrom;
    uint32_t position;
    uint16_t nalleles;
};

static inline void set_tpl_str(tpl_string *dst, const string *src) {
    dst->len = src->len;
    dst->str = src->str;
}

static inline void set_str(string *dst, const tpl_string *src) {
    dst->len = src->len;
    dst->str = src->str;
}

struct BGenVI *bgen_new_index(const struct BGenFile *bgen) {
    struct BGenVI *vi;

    vi = malloc(sizeof(struct BGenVI));

    vi->filepath = bgen_strdup(bgen->filepath);
    vi->compression = bgen->compression;
    vi->layout = bgen->layout;
    vi->nsamples = bgen->nsamples;
    vi->nvariants = bgen->nvariants;

    vi->start = malloc(bgen->nvariants * sizeof(inti));

    return vi;
}

inti append_variants(inti, struct BGenVar *, struct Buffer *);
inti append_alleles(inti, struct BGenVar *, struct Buffer *);
inti append_genotype_offsets(inti, inti *, struct Buffer *);
size_t read_variants(void *mem, struct BGenVar *variants);
size_t read_alleles(void *mem, struct BGenVar *variants);
size_t read_genotype_offsets(void *mem, const struct BGenFile *bgen,
                             struct BGenVI **vi);

inti bgen_store_variants(const struct BGenFile *bgen, struct BGenVar *variants,
                         struct BGenVI *index, const byte *fp) {

    struct Buffer *b;

    b = buffer_create();

    append_variants(bgen->nvariants, variants, b);
    append_alleles(bgen->nvariants, variants, b);
    append_genotype_offsets(bgen->nvariants, index->start, b);

    buffer_store(fp, b);
    buffer_destroy(b);

    return SUCCESS;
}

struct BGenVar *bgen_load_variants(const struct BGenFile *bgen, const byte *fp,
                                   struct BGenVI **vi) {
    struct Buffer *b;
    struct BGenVar *variants;
    void *mem;
    size_t read_len;

    b = buffer_create();

    if (buffer_load(fp, b)) {
        fprintf(stderr, "Could not load buffer for %s.\n", fp);
    }
    mem = buffer_base(b);

    variants = malloc(bgen->nvariants * sizeof(struct BGenVar));

    read_len = read_variants(mem, variants);
    mem += read_len;

    read_len = read_alleles(mem, variants);
    mem += read_len;

    read_genotype_offsets(mem, bgen, vi);
    mem += read_len;

    buffer_destroy(b);
    return variants;
}

inti append_genotype_offsets(inti nvariants, inti *offsets, struct Buffer *b) {
    tpl_node *tn;

    tn = tpl_map("I#", offsets, nvariants);
    tpl_pack(tn, 0);
    tpl_append_buffer(tn, b);

    return 0;
}

inti append_variants(inti nvariants, struct BGenVar *variants,
                     struct Buffer *b) {
    inti i;
    tpl_node *tn;
    struct TPLVar tpl_variant;

    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &tpl_variant);

    for (i = 0; i < nvariants; ++i) {
        set_tpl_str(&tpl_variant.id, &variants[i].id);
        set_tpl_str(&tpl_variant.rsid, &variants[i].rsid);
        set_tpl_str(&tpl_variant.chrom, &variants[i].chrom);

        tpl_variant.position = variants[i].position;
        tpl_variant.nalleles = variants[i].nalleles;

        tpl_pack(tn, 1);
    }

    tpl_append_buffer(tn, b);

    return 0;
}

inti append_alleles(inti nvariants, struct BGenVar *variants,
                    struct Buffer *b) {
    inti i, j;
    tpl_node *tn;
    tpl_string allele_id;

    tn = tpl_map("A(A(S(vs)))", &allele_id);

    for (i = 0; i < nvariants; ++i) {
        for (j = 0; j < variants[i].nalleles; ++j) {
            set_tpl_str(&allele_id, variants[i].allele_ids + j);
            tpl_pack(tn, 2);
        }
        tpl_pack(tn, 1);
    }

    tpl_append_buffer(tn, b);

    return 0;
}

size_t read_variants(void *mem, struct BGenVar *variants) {
    tpl_node *tn;
    struct TPLVar v;
    size_t i;
    uint64_t block_size;

    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &v);

    block_size = *((uint64_t *)mem);
    mem += sizeof(uint64_t);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not load variants block.");
        return 0;
    }

    i = 0;
    while (tpl_unpack(tn, 1) > 0) {
        set_str(&variants[i].id, &v.id);
        set_str(&variants[i].rsid, &v.rsid);
        set_str(&variants[i].chrom, &v.chrom);

        variants[i].position = v.position;
        variants[i].nalleles = v.nalleles;

        ++i;
    }

    tpl_free(tn);

    return block_size + sizeof(uint64_t);
}

size_t read_alleles(void *mem, struct BGenVar *variants) {
    tpl_node *tn;
    tpl_string allele_id;
    size_t i, j;
    uint64_t block_size;

    tn = tpl_map("A(A(S(vs)))", &allele_id);

    block_size = *((uint64_t *)mem);
    mem += sizeof(uint64_t);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not load alleles block.");
        return 0;
    }

    i = 0;
    while (tpl_unpack(tn, 1) > 0) {
        variants[i].allele_ids = malloc(variants[i].nalleles * sizeof(string));
        j = 0;
        while (tpl_unpack(tn, 2) > 0) {
            set_str(variants[i].allele_ids + j, &allele_id);
            ++j;
        }

        ++i;
    }

    tpl_free(tn);

    return block_size + sizeof(uint64_t);
}

size_t read_genotype_offsets(void *mem, const struct BGenFile *bgen,
                             struct BGenVI **vi) {
    tpl_node *tn;
    uint64_t block_size;

    *vi = bgen_new_index(bgen);
    tn = tpl_map("I#", (*vi)->start, bgen->nvariants);

    block_size = *((uint64_t *)mem);
    mem += sizeof(uint64_t);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not load genotype offsets.");
        return 0;
    }

    tpl_unpack(tn, 0);
    tpl_free(tn);

    return 0;
}
