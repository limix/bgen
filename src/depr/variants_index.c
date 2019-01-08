/* This module reads and writes the bgen index.
 *
 * A bgen index file is defined as follows.
 *
 * [ header block of 13 characters ], : "bgen index 02"
 * [
 *   uint16_t, str,  : variant id
 *   uint16_t, str,  : variant rsid
 *   uint16_t, str,  : variant chrom
 *   uint32_t,       : genetic position
 *   uint16_t,       : number of alleles
 *   [
 *     uint16_t, str : allele id
 *   ], ...
 * ], ...
 * [
 *   uint64_t : variant offset
 * ], ...
 *
 * Note, however, that the serialization is done by the TPL
 * library, which means that the resulting file will have
 * TPL fields not mentioned above.
 */

#include "depr/variants_index.h"
#include "bgen.h"
#include "buffer.h"
#include "depr/tpl.h"
#include "file.h"
#include "mem.h"
#include "tpl/tpl.h"
#include "zip/zstd.h"

#define SET_STR(dst, src)                                                              \
    (dst)->len = (src)->len;                                                           \
    (dst)->str = (src)->str;

typedef struct tpl_string
{
    uint16_t len;
    char *str;
} tpl_string;

struct TPLVar
{
    tpl_string id;
    tpl_string rsid;
    tpl_string chrom;
    uint32_t position;
    uint16_t nalleles;
};

struct bgen_vi *new_variants_index(const struct bgen_file *bgen)
{
    struct bgen_vi *vi;

    vi = malloc(sizeof(struct bgen_vi));

    vi->filepath = strdup(bgen->filepath);
    vi->compression = bgen->compression;
    vi->layout = bgen->layout;
    vi->nsamples = bgen->nsamples;
    vi->nvariants = bgen->nvariants;
    vi->max_nalleles = 0;

    vi->start = malloc(bgen->nvariants * sizeof(uint64_t));

    return vi;
}

int append_header(struct Buffer *);
int append_variants(size_t, struct bgen_var *, struct Buffer *);
int append_alleles(size_t, struct bgen_var *, struct Buffer *);
int append_genotype_offsets(size_t, uint64_t *, struct Buffer *);
size_t read_header(void *mem, char *header);
size_t read_variants(void *mem, struct bgen_var *variants);
size_t read_alleles(void *mem, struct bgen_var *variants);
size_t read_genotype_offsets(void *mem, const struct bgen_file *bgen,
                             struct bgen_vi **vi);
void *read_mem_size(void *mem, uint64_t *block_size);

int bgen_store_variants_metadata(const struct bgen_file *bgen,
                                 struct bgen_var *variants, struct bgen_vi *index,
                                 const char *fp)
{

    struct Buffer *b;

    b = buffer_create();

    if (append_header(b))
        return 1;
    if (append_variants(bgen->nvariants, variants, b))
        return 1;
    if (append_alleles(bgen->nvariants, variants, b))
        return 1;
    if (append_genotype_offsets(bgen->nvariants, index->start, b))
        return 1;

    buffer_store(fp, b);
    buffer_destroy(b);

    return 0;
}

struct bgen_var *bgen_load_variants_metadata(const struct bgen_file *bgen,
                                             const char *filepath, struct bgen_vi **vi,
                                             int verbose)
{
    struct Buffer *b;
    struct bgen_var *variants = NULL;
    char header[BGEN_HEADER_LENGTH + 1];
    void *mem;
    size_t read_len;

    b = buffer_create();

    if (buffer_load(filepath, b, verbose)) {
        error("Could not load buffer for %s", filepath);
        goto err;
    }
    mem = buffer_base(b);

    read_len = read_header(mem, header);
    if (read_len == 0) {
        error("Could not read the header of %s", filepath);
        goto err;
    }
    if (!(header[11] == '0' && header[11] == '1'))
        mem = (char *)mem + read_len;

    variants = dalloc(bgen->nvariants * sizeof(struct bgen_var));
    if (!variants)
        goto err;

    read_len = read_variants(mem, variants);
    mem = (char *)mem + read_len;

    read_len = read_alleles(mem, variants);
    mem = (char *)mem + read_len;

    read_genotype_offsets(mem, bgen, vi);

    buffer_destroy(b);
    return variants;

err:
    buffer_destroy(b);
    free_nul(variants);
    return NULL;
}

int bgen_create_variants_metadata_file(const char *bgen_fp, const char *index_fp,
                                       int verbose)
{
    struct bgen_file *bgen;
    struct bgen_var *variants;
    struct bgen_vi *index;

    if ((bgen = bgen_open(bgen_fp)) == NULL)
        return 1;

    if ((variants = bgen_read_variants_metadata(bgen, &index, verbose)) == NULL)
        return 1;

    if (bgen_store_variants_metadata(bgen, variants, index, index_fp))
        return 1;

    bgen_free_variants_metadata(bgen, variants);
    bgen_free_index(index);

    bgen_close(bgen);

    return 0;
}

int append_genotype_offsets(size_t nvariants, uint64_t *offsets, struct Buffer *b)
{
    tpl_node *tn;

    // Fixed-length array of 64-bit unsigned int
    tn = tpl_map("U#", offsets, nvariants);
    tpl_pack(tn, 0);
    tpl_append_buffer(tn, b);

    return 0;
}

int append_header(struct Buffer *b)
{
    /* Write the header.
     *
     * Suppose this implements bgen index version 2.
     * The header will have 13 characters: bgen index 02
     */

    char header[] = {BGEN_INDEX_NAME BGEN_INDEX_VERSION};

    tpl_node *tn;

    // Fixed-length string
    tn = tpl_map("c#", header, BGEN_HEADER_LENGTH);
    tpl_pack(tn, 0);
    tpl_append_buffer(tn, b);

    return 0;
}

int append_variants(size_t nvariants, struct bgen_var *variants, struct Buffer *b)
{
    /*
     * Write a list of variant metadata.
     *
     * Each element of that list contain
     *
     *   uint16_t, str,  : variant id
     *   uint16_t, str,  : variant rsid
     *   uint16_t, str,  : variant chrom
     *   uint32_t,       : genetic position
     *   uint16_t,       : number of alleles
     */
    size_t i;
    tpl_node *tn;
    struct TPLVar tpl_variant;

    // Variable-length array of structs.
    // Those structs have each three inner structs containing
    // a 16-bit unsigned int and a string, plus a 32-bit unsigned int
    // and a 16-bit unsigned int.
    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &tpl_variant);

    for (i = 0; i < nvariants; ++i) {
        SET_STR(&tpl_variant.id, &variants[i].id);
        SET_STR(&tpl_variant.rsid, &variants[i].rsid);
        SET_STR(&tpl_variant.chrom, &variants[i].chrom);

        tpl_variant.position = variants[i].position;
        tpl_variant.nalleles = variants[i].nalleles;

        tpl_pack(tn, 1);
    }

    tpl_append_buffer(tn, b);

    return 0;
}

int append_alleles(size_t nvariants, struct bgen_var *variants, struct Buffer *b)
{
    /* Write a list of allele ids.
     *
     * Each element of that list contain
     *
     *     uint16_t, str : allele id
     */
    size_t i, j;
    tpl_node *tn;
    tpl_string allele_id;

    tn = tpl_map("A(A(S(vs)))", &allele_id);

    for (i = 0; i < nvariants; ++i) {
        for (j = 0; j < (size_t)variants[i].nalleles; ++j) {
            SET_STR(&allele_id, variants[i].allele_ids + j);
            tpl_pack(tn, 2);
        }
        tpl_pack(tn, 1);
    }

    tpl_append_buffer(tn, b);

    return 0;
}

size_t read_header(void *mem, char *header)
{
    tpl_node *tn;
    uint64_t block_size;

    tn = tpl_map("c#", header, BGEN_HEADER_LENGTH);

    block_size = *((uint64_t *)mem);
    mem = (char *)mem + sizeof(uint64_t);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not tpl_load header block.\n");
        return 0;
    }

    if (tpl_unpack(tn, 0)) {
        strncpy(header, BGEN_INDEX_NAME BGEN_INDEX_VERSION "\0",
                BGEN_HEADER_LENGTH + 1);
        return (size_t)(block_size + sizeof(uint64_t));
    }

    tpl_free(tn);

    return (size_t)(block_size + sizeof(uint64_t));
}

size_t read_variants(void *mem, struct bgen_var *variants)
{
    tpl_node *tn;
    struct TPLVar v;
    size_t i;
    uint64_t block_size;

    tn = tpl_map("A(S($(vs)$(vs)$(vs)uv))", &v);
    mem = read_mem_size(mem, &block_size);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not load variants block.\n");
        return 0;
    }

    i = 0;
    while (tpl_unpack(tn, 1) > 0) {
        SET_STR(&variants[i].id, &v.id);
        SET_STR(&variants[i].rsid, &v.rsid);
        SET_STR(&variants[i].chrom, &v.chrom);

        variants[i].position = v.position;
        variants[i].nalleles = v.nalleles;

        ++i;
    }

    tpl_free(tn);

    return (size_t)(block_size + sizeof(uint64_t));
}

size_t read_alleles(void *mem, struct bgen_var *variants)
{
    tpl_node *tn;
    tpl_string allele_id;
    size_t i;
    uint64_t block_size;

    tn = tpl_map("A(A(S(vs)))", &allele_id);
    mem = read_mem_size(mem, &block_size);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not load alleles block.\n");
        return 0;
    }

    i = 0;
    while (tpl_unpack(tn, 1) > 0) {
        variants[i].allele_ids = malloc(variants[i].nalleles * sizeof(struct bgen_str));
        size_t j = 0;
        while (tpl_unpack(tn, 2) > 0) {
            SET_STR(variants[i].allele_ids + j, &allele_id);
            ++j;
        }

        ++i;
    }

    tpl_free(tn);

    return (size_t)(block_size + sizeof(uint64_t));
}

size_t read_genotype_offsets(void *mem, const struct bgen_file *bgen,
                             struct bgen_vi **vi)
{
    tpl_node *tn;
    uint64_t block_size;

    *vi = new_variants_index(bgen);
    tn = tpl_map("U#", (*vi)->start, bgen->nvariants);
    mem = read_mem_size(mem, &block_size);

    if (tpl_load(tn, TPL_MEM, mem, block_size)) {
        fprintf(stderr, "Could not load genotype offsets.\n");
        return 0;
    }

    tpl_unpack(tn, 0);
    tpl_free(tn);

    return 0;
}

void *read_mem_size(void *mem, uint64_t *block_size)
{
    *block_size = *((uint64_t *)mem);
    return (char *)mem + sizeof(uint64_t);
}
