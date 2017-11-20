#include "bgen/bgen.h"
#include "bgen/number.h"
#include "tpl/tpl.h"
#include "util/file.h"
#include "util/mem.h"
#include "omem/omem.h"
#include "variants_index.h"

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

static inline void tplstr_get_str(tpl_string *dst, const string *src) {
    dst->len = src->len;
    dst->str = src->str;
}

static inline void str_get_tplstr(string *dst, const tpl_string *src) {
    dst->len = src->len;
    dst->str = src->str;
}

struct BGenVI *bgen_new_index(const struct BGenFile *bgen)
{
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

void dump2file(tpl_node *tn, FILE *file)
{
    byte *mem;
    size_t mem_size;
    uint64_t mem_size_;

    tpl_dump(tn, TPL_MEM, &mem, &mem_size);
    mem_size_ = mem_size;

    fwrite(&mem_size_, 1, 8, file);
    fwrite(mem, 1, mem_size_, file);

    free(mem);
}

inti dump_variants(inti nvariants, struct BGenVar *vars, FILE *file);
inti dump_alleles(inti nvariants, struct BGenVar *vars, FILE *file);
inti dump_genotype_offset(inti nvariants, inti *genotype_offset, FILE *file);

inti bgen_store_variants(const struct BGenFile *bgen, struct BGenVar *vars,
                         struct BGenVI *index, const byte *fp) {
    FILE *file, *buffer;
    void *base;
    size_t size;
    omem fm;
    unsigned char header[] = {178, 125, 13, 247};

    omem_init(&fm);
    printf("Ponto 0\n"); fflush(stdout);
    buffer = omem_open(&fm, "wb");
    printf("Ponto 1\n"); fflush(stdout);

    //if (file == NULL) {
    //    fprintf(stderr, "Could not open %s", fp);
    //    perror(fp);
    //    return FAIL;
    //}

    if (fwrite(header, 1, 4, buffer) != 4) {
        //fprintf(stderr, "Could not write to %s", fp);
        //perror(NULL);
        printf("Ponto 2\n"); fflush(stdout);
        return FAIL;
    }

    dump_variants(bgen->nvariants, vars, buffer);
    printf("Ponto 3\n"); fflush(stdout);
    dump_alleles(bgen->nvariants, vars, buffer);
    printf("Ponto 4\n"); fflush(stdout);
    dump_genotype_offset(bgen->nvariants, index->start, buffer);
    printf("Ponto 5\n"); fflush(stdout);

    fflush(buffer);
    //size = ftell(buffer);
    //fseek(buffer, 0, SEEK_SET);
    omem_mem(&fm, &base, &size);

    file = fopen(fp, "wb");
    fwrite(base, 1, size, file);
    fclose(file);

    fclose(buffer);
    omem_term(&fm);

    return SUCCESS;
}

struct BGenVar *bgen_load_variants(const struct BGenFile *bgen, const byte *fp,
                            struct BGenVI **vi) {
    inti i, j;
    FILE *file;
    unsigned char header[4];
    tpl_node *tn;
    size_t mem_size;
    byte *mem;
    uint64_t mem_size_64bits;
    struct TPLVar tpl_variant;
    tpl_string allele_id;
    struct BGenVar *vars;

    *vi = bgen_new_index(bgen);
    vars = malloc(bgen->nvariants * sizeof(struct BGenVar));

    file = fopen(fp, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open: %s\n", fp);
        return NULL;
    }

    if (fread(header, 1, 4, file) != 4) {
        fprintf(stderr, "Could not read file header.\n");
        return NULL;
    }

    if (fread(&mem_size_64bits, 1, 8, file) != 8) {
        fprintf(stderr, "Could not read the size of the variants block.\n");
        return NULL;
    }

    mem_size = mem_size_64bits;
    mem = malloc(mem_size);

    if (fread(mem, 1, mem_size, file) != mem_size) {
        fprintf(stderr, "Could not read variants block.\n");
        return NULL;
    }
    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &tpl_variant);

    if (tn == NULL) {
        fprintf(stderr, "Could not map variants block.");
        return NULL;
    }

    if (tpl_load(tn, TPL_MEM, mem, mem_size)) {
        fprintf(stderr, "Could not load variants block.");
        return NULL;
    }

    i = 0;
    while (tpl_unpack(tn, 1) > 0) {
        str_get_tplstr(&vars[i].id, &tpl_variant.id);
        str_get_tplstr(&vars[i].rsid, &tpl_variant.rsid);
        str_get_tplstr(&vars[i].chrom, &tpl_variant.chrom);

        vars[i].position = tpl_variant.position;
        vars[i].nalleles = tpl_variant.nalleles;

        ++i;
    }

    free(mem);
    tpl_free(tn);

    if (fread(&mem_size_64bits, 1, 8, file) != 8) {
        fprintf(stderr, "Could not read the size of the allelles block.\n");
        return NULL;
    }

    mem_size = mem_size_64bits;
    mem = malloc(mem_size);

    if (fread(mem, 1, mem_size, file) != mem_size) {
        fprintf(stderr, "Could not read allelles block.\n");
        return NULL;
    }

    tn = tpl_map("A(A(S(vs)))", &allele_id);
    if (tn == NULL) {
        fprintf(stderr, "Could not map alleles block.");
        return NULL;
    }

    if (tpl_load(tn, TPL_MEM, mem, mem_size)) {
        fprintf(stderr, "Could not load alleles block.");
        return NULL;
    }

    i = 0;
    while (tpl_unpack(tn, 1) > 0) {
        vars[i].allele_ids = malloc(vars[i].nalleles * sizeof(string));
        j = 0;
        while (tpl_unpack(tn, 2) > 0) {
            str_get_tplstr(vars[i].allele_ids + j, &allele_id);
            ++j;
        }

        ++i;
    }

    tpl_free(tn);
    free(mem);

    if (fread(&mem_size_64bits, 1, 8, file) != 8) {
        fprintf(stderr,
                "Could not read the size of the genotype offsets block.\n");
        return NULL;
    }

    mem_size = mem_size_64bits;
    mem = malloc(mem_size);

    if (fread(mem, 1, mem_size, file) != mem_size) {
        fprintf(stderr, "Could not read genotype offsets block.\n");
        return NULL;
    }

    tn = tpl_map("I#", (*vi)->start, bgen->nvariants);
    if (tn == NULL) {
        fprintf(stderr, "Could not map genotype offsets.");
        return NULL;
    }

    if (tpl_load(tn, TPL_MEM, mem, mem_size)) {
        fprintf(stderr, "Could not load genotype offsets.");
        return NULL;
    }

    tpl_unpack(tn, 0);

    tpl_free(tn);
    free(mem);

    fclose(file);

    return vars;
}

inti dump_genotype_offset(inti nvariants, inti *genotype_offset, FILE *file)
{
    tpl_node *tn;

    tn = tpl_map("I#", genotype_offset, nvariants);

    tpl_pack(tn, 0);
    dump2file(tn, file);
    tpl_free(tn);

    return SUCCESS;
}

inti dump_variants(inti nvariants, struct BGenVar *vars, FILE *file) {
    inti i;
    tpl_node *tn;
    struct TPLVar tpl_variant;

    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &tpl_variant);

    for (i = 0; i < nvariants; ++i) {
        tplstr_get_str(&tpl_variant.id, &vars[i].id);
        tplstr_get_str(&tpl_variant.rsid, &vars[i].rsid);
        tplstr_get_str(&tpl_variant.chrom, &vars[i].chrom);

        tpl_variant.position = vars[i].position;
        tpl_variant.nalleles = vars[i].nalleles;

        tpl_pack(tn, 1);
    }

    dump2file(tn, file);

    tpl_free(tn);

    return SUCCESS;
}

inti dump_alleles(inti nvariants, struct BGenVar *vars, FILE *file) {
    inti i, j;
    tpl_node *tn;
    tpl_string allele_id;

    tn = tpl_map("A(A(S(vs)))", &allele_id);

    for (i = 0; i < nvariants; ++i) {
        for (j = 0; j < vars[i].nalleles; ++j) {
            tplstr_get_str(&allele_id, vars[i].allele_ids + j);
            tpl_pack(tn, 2);
        }
        tpl_pack(tn, 1);
    }

    dump2file(tn, file);
    tpl_free(tn);

    return SUCCESS;
}
