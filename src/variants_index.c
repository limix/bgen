#include "bgen.h"
#include "bgen_file.h"
#include "tpl/tpl.h"
#include "util/buffer.h"
#include "util/mem.h"
#include "util/tpl.h"
#include "variants_index.h"
#include "zip/zstd_wrapper.h"

typedef struct tpl_string {
  uint16_t len;
  char *str;
} tpl_string;

struct TPLVar {
  tpl_string id;
  tpl_string rsid;
  tpl_string chrom;
  uint32_t position;
  uint16_t nalleles;
};

static inline void set_tpl_str(tpl_string *dst, const bgen_string *src) {
  dst->len = src->len;
  dst->str = src->str;
}

static inline void set_str(bgen_string *dst, const tpl_string *src) {
  dst->len = src->len;
  dst->str = src->str;
}

struct BGenVI *new_variants_index(const struct BGenFile *bgen) {
  struct BGenVI *vi;

  vi = malloc(sizeof(struct BGenVI));

  vi->filepath = bgen_strdup(bgen->filepath);
  vi->compression = bgen->compression;
  vi->layout = bgen->layout;
  vi->nsamples = bgen->nsamples;
  vi->nvariants = bgen->nvariants;

  vi->start = malloc(bgen->nvariants * sizeof(uint64_t));

  return vi;
}

int append_variants(size_t, struct BGenVar *, struct Buffer *);
int append_alleles(size_t, struct BGenVar *, struct Buffer *);
int append_genotype_offsets(size_t, uint64_t *, struct Buffer *);
size_t read_variants(void *mem, struct BGenVar *variants);
size_t read_alleles(void *mem, struct BGenVar *variants);
size_t read_genotype_offsets(void *mem, const struct BGenFile *bgen,
                             struct BGenVI **vi);

int bgen_store_variants(const struct BGenFile *bgen, struct BGenVar *variants,
                        struct BGenVI *index, const char *fp) {

  struct Buffer *b;

  b = buffer_create();

  append_variants(bgen->nvariants, variants, b);
  append_alleles(bgen->nvariants, variants, b);
  append_genotype_offsets(bgen->nvariants, index->start, b);

  buffer_store(fp, b);
  buffer_destroy(b);

  return 0;
}

struct BGenVar *bgen_load_variants(const struct BGenFile *bgen, const char *fp,
                                   struct BGenVI **vi, int verbose) {
  struct Buffer *b;
  struct BGenVar *variants;
  void *mem;
  size_t read_len;

  b = buffer_create();

  if (buffer_load(fp, b, verbose)) {
    fprintf(stderr, "Could not load buffer for %s.\n", fp);
  }
  mem = buffer_base(b);

  variants = malloc(bgen->nvariants * sizeof(struct BGenVar));

  read_len = read_variants(mem, variants);
  mem = (char *)mem + read_len;

  read_len = read_alleles(mem, variants);
  mem = (char *)mem + read_len;

  read_genotype_offsets(mem, bgen, vi);
  mem = (char *)mem + read_len;

  buffer_destroy(b);
  return variants;
}

int bgen_create_variants_file(const char *bgen_fp, const char *index_fp,
                                    int verbose) {
  struct BGenFile *bgen;
  struct BGenVar *variants;
  struct BGenVI *index;

  if ((bgen = bgen_open(bgen_fp)) == NULL)
    return 1;

  if ((variants = bgen_read_variants(bgen, &index, verbose)) == NULL)
    return 1;

  if (bgen_store_variants(bgen, variants, index, index_fp))
    return 1;

  bgen_free_variants(bgen, variants);
  bgen_free_index(index);

  bgen_close(bgen);

  return 0;
}

int append_genotype_offsets(size_t nvariants, uint64_t *offsets,
                            struct Buffer *b) {
  tpl_node *tn;

  tn = tpl_map("U#", offsets, nvariants);
  tpl_pack(tn, 0);
  tpl_append_buffer(tn, b);

  return 0;
}

int append_variants(size_t nvariants, struct BGenVar *variants,
                    struct Buffer *b) {
  size_t i;
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

int append_alleles(size_t nvariants, struct BGenVar *variants,
                   struct Buffer *b) {
  size_t i, j;
  tpl_node *tn;
  tpl_string allele_id;

  tn = tpl_map("A(A(S(vs)))", &allele_id);

  for (i = 0; i < nvariants; ++i) {
    for (j = 0; j < (size_t)variants[i].nalleles; ++j) {
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
  mem = (char *)mem + sizeof(uint64_t);

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

  return (size_t)(block_size + sizeof(uint64_t));
}

size_t read_alleles(void *mem, struct BGenVar *variants) {
  tpl_node *tn;
  tpl_string allele_id;
  size_t i, j;
  uint64_t block_size;

  tn = tpl_map("A(A(S(vs)))", &allele_id);

  block_size = *((uint64_t *)mem);
  mem = (char *)mem + sizeof(uint64_t);

  if (tpl_load(tn, TPL_MEM, mem, block_size)) {
    fprintf(stderr, "Could not load alleles block.");
    return 0;
  }

  i = 0;
  while (tpl_unpack(tn, 1) > 0) {
    variants[i].allele_ids = malloc(variants[i].nalleles * sizeof(bgen_string));
    j = 0;
    while (tpl_unpack(tn, 2) > 0) {
      set_str(variants[i].allele_ids + j, &allele_id);
      ++j;
    }

    ++i;
  }

  tpl_free(tn);

  return (size_t)(block_size + sizeof(uint64_t));
}

size_t read_genotype_offsets(void *mem, const struct BGenFile *bgen,
                             struct BGenVI **vi) {
  tpl_node *tn;
  uint64_t block_size;

  *vi = new_variants_index(bgen);
  tn = tpl_map("U#", (*vi)->start, bgen->nvariants);

  block_size = *((uint64_t *)mem);
  mem = (char *)mem + sizeof(uint64_t);

  if (tpl_load(tn, TPL_MEM, mem, block_size)) {
    fprintf(stderr, "Could not load genotype offsets.");
    return 0;
  }

  tpl_unpack(tn, 0);
  tpl_free(tn);

  return 0;
}
