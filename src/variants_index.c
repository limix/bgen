#include "bgen/bgen.h"
#include "bgen/number.h"
#include "tpl/tpl.h"
#include "util/file.h"
#include "util/mem.h"
#include "variant_indexing.h"

typedef struct tpl_string {
    uint16_t len;
    byte *str;
} tpl_string;

typedef struct TPLVariant {
    tpl_string id;
    tpl_string rsid;
    tpl_string chrom;
    uint32_t position;
    uint16_t nalleles;
} TPLVariant;

static inline void tplstr_get_str(tpl_string *dst, const string *src) {
    dst->len = src->len;
    dst->str = src->str;
}

static inline void str_get_tplstr(string *dst, const tpl_string *src) {
    dst->len = src->len;
    dst->str = src->str;
}

inti dump_variants(inti nvariants, Variant *vars, FILE *file);
inti dump_alleles(inti nvariants, Variant *vars, FILE *file);

inti bgen_store_variants(const BGenFile *bgen, Variant *vars, const byte *fp) {
    FILE *file;
    unsigned char header[] = {178, 125, 13, 247};

    file = fopen(fp, "wb");
    if (file == NULL) {
        perror(fp);
        return FAIL;
    }

    if (fwrite(header, 1, 4, file) != 4) {
        fprintf(stderr, "Could not write to %s\n", fp);
        return FAIL;
    }

    dump_variants(bgen->nvariants, vars, file);
    dump_alleles(bgen->nvariants, vars, file);

    fclose(file);
    return SUCCESS;
}

Variant *bgen_load_variants(const BGenFile *bgen, const byte *fp,
                            VariantIndexing **vi) {
    inti i, j;
    FILE *file;
    unsigned char header[4];
    tpl_node *tn;
    size_t mem_size;
    byte *mem;
    uint64_t mem_size_64bits;
    TPLVariant tpl_variant;
    tpl_string allele_id;
    Variant *vars;

    vars = NULL;
    *vi = malloc(sizeof(VariantIndexing));
    (*vi)->filepath = bgen_strdup(bgen->filepath);
    (*vi)->compression = bgen->compression;
    (*vi)->layout = bgen->layout;
    (*vi)->nsamples = bgen->nsamples;
    (*vi)->nvariants = bgen->nvariants;
    vars = malloc(bgen->nvariants * sizeof(Variant));
    (*vi)->start = malloc(bgen->nvariants * sizeof(inti));

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

        // printf("Index[%lld]\n", i);
        // printf("  ID   : %s\n", vars[i].id.str);
        // printf("  RSID : %s\n", vars[i].rsid.str);
        // printf("  Chrom: %s\n", vars[i].chrom.str);
        // printf("  Pos  : %lld\n", vars[i].position);
        // printf("  Nalel: %lld\n", vars[i].nalleles);

        ++i;
    }

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
        // printf("Index[%lld]\n", i);
        vars[i].allele_ids = malloc(vars[i].nalleles * sizeof(string));
        j = 0;
        while (tpl_unpack(tn, 2) > 0) {
            str_get_tplstr(vars[i].allele_ids + j, &allele_id);
            // printf("  ID: %s\n", vars[i].allele_ids[j].str);
            ++j;
        }

        ++i;
    }

    tpl_free(tn);
    fclose(file);

    return vars;
}

inti dump_variants(inti nvariants, Variant *vars, FILE *file) {
    inti i;
    tpl_node *tn;
    byte *mem;
    size_t mem_size;
    uint64_t mem_size_64bits;
    TPLVariant tpl_variant;

    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &tpl_variant);

    for (i = 0; i < nvariants; ++i) {
        tplstr_get_str(&tpl_variant.id, &vars[i].id);
        tplstr_get_str(&tpl_variant.rsid, &vars[i].rsid);
        tplstr_get_str(&tpl_variant.chrom, &vars[i].chrom);

        tpl_variant.position = vars[i].position;
        tpl_variant.nalleles = vars[i].nalleles;

        tpl_pack(tn, 1);
    }

    tpl_dump(tn, TPL_MEM, &mem, &mem_size);
    mem_size_64bits = mem_size;
    fwrite(&mem_size_64bits, 1, 8, file);
    fwrite(mem, 1, mem_size_64bits, file);
    free(mem);
    tpl_free(tn);

    return SUCCESS;
}

inti dump_alleles(inti nvariants, Variant *vars, FILE *file) {
    inti i, j;
    tpl_node *tn;
    byte *mem;
    size_t mem_size;
    uint64_t mem_size_64bits;
    tpl_string allele_id;

    tn = tpl_map("A(A(S(vs)))", &allele_id);

    for (i = 0; i < nvariants; ++i) {
        for (j = 0; j < vars[i].nalleles; ++j) {
            tplstr_get_str(&allele_id, vars[i].allele_ids + j);
            tpl_pack(tn, 2);
        }
        tpl_pack(tn, 1);
    }

    tpl_dump(tn, TPL_MEM, &mem, &mem_size);
    mem_size_64bits = mem_size;
    fwrite(&mem_size_64bits, 1, 8, file);
    fwrite(mem, 1, mem_size_64bits, file);
    free(mem);
    tpl_free(tn);

    return SUCCESS;
}
