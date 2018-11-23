#define BGEN_API_EXPORTS

#include "metafile.h"
#include "bgen_file.h"
#include "bits.h"
#include "endian.h"
#include "io.h"
#include "mem.h"
#include "str.h"
#include "variant.h"

struct bgen_cmf_next {
    struct bgen_file *bgen;
    uint32_t nvariants;
};

/* node for creating metadata file */
struct cmf {
    FILE *fp;
    uint32_t npartitions;
    uint32_t nvariants;
    uint64_t *offset;
};

/* variant metadata index */
struct bgen_idx {
    char *filepath;
    uint32_t nvariants;
    uint64_t metasize;
    uint32_t npartitions;
    uint64_t *ppos;
};

struct bgen_var *bgen_next_variant_metadata(uint64_t *genotype_offset, void *ctx) {

    struct bgen_var *variant = malloc(sizeof(struct bgen_var));
    struct bgen_cmf_next *self = ctx;
    uint32_t length;

    if (self->nvariants == 0)
        return NULL;

    if (read_variant(self->bgen, variant))
        goto err;

    *genotype_offset = ftell(self->bgen->file);

    if (fread(&length, 1, 4, self->bgen->file) < 4)
        goto err;

    if (fseek(self->bgen->file, length, SEEK_CUR)) {
        perror("Could not jump to the next variant ");
        goto err;
    }

    --(self->nvariants);
    return variant;
err:
    return NULL;
}

struct cmf *create_metafile(const char *filepath, uint32_t nvars, uint32_t nparts) {

    struct cmf *c = dalloc(sizeof(struct cmf));
    if (!c)
        goto err;

    c->offset = NULL;
    c->fp = NULL;

    if (!is_little_endian()) {
        error("This program does not support big-endian platforms yet.");
        goto err;
    }

    if (!(c->fp = fopen(filepath, "wb"))) {
        perror("Could not create file ");
        goto err;
    }

    if (fwrite1(BGEN_IDX_NAME BGEN_IDX_VER, BGEN_HDR_LEN * sizeof(char), c->fp))
        goto err;

    if (fwrite1(&nvars, sizeof(uint32_t), c->fp))
        goto err;

    if (fseek(c->fp, sizeof(uint64_t), SEEK_CUR))
        goto err;

    c->npartitions = nparts;
    c->nvariants = nvars;

    return c;
err:
    if (c)
        fclose_nul(c->fp);
    free_nul(c);
    return NULL;
}

uint64_t write_metafile_var(FILE *fp, const struct bgen_var *v, uint64_t offset) {
    long start = ftell(fp);

    fwrite_ui64(fp, offset, 8);
    fwrite_str(fp, &(v->id), 2);
    fwrite_str(fp, &(v->rsid), 2);
    fwrite_str(fp, &(v->chrom), 2);
    fwrite_int(fp, v->position, 4);
    fwrite_int(fp, v->nalleles, 2);

    for (size_t j = 0; j < v->nalleles; ++j)
        fwrite_str(fp, v->allele_ids + j, 4);

    return ftell(fp) - start;
}

int write_metafile(struct cmf *cmf, struct bgen_var *(*next)(uint64_t *, void *),
                   void *cb_args, int verbose) {

    struct bgen_var *v = NULL;
    uint64_t size;
    uint64_t offset;

    cmf->offset = dalloc(sizeof(uint64_t) * (cmf->npartitions + 1));
    if (cmf->offset == NULL)
        goto err;

    cmf->offset[0] = 0;
    size_t i = 0, j = 0;
    while ((v = next(&offset, cb_args)) != NULL) {

        if ((size = write_metafile_var(cmf->fp, v, offset)) == 0)
            goto err;

        if (i % (cmf->nvariants / cmf->npartitions) == 0) {
            ++j;
            cmf->offset[j] = cmf->offset[j - 1];
        }

        cmf->offset[j] += size;
        ++i;
        v = free_nul(v);
    }

    fwrite_ui32(cmf->fp, cmf->npartitions, sizeof(cmf->npartitions));

    for (i = 0; i < cmf->npartitions; ++i)
        fwrite_ui64(cmf->fp, cmf->offset[i], sizeof(cmf->offset[i]));

    fseek(cmf->fp, BGEN_HDR_LEN + sizeof(uint32_t), SEEK_SET);
    fwrite_ui64(cmf->fp, cmf->offset[cmf->npartitions], 4);

    return 0;

err:
    cmf->offset = free_nul(cmf->offset);
    free_nul(v);
    return 1;
}

int close_metafile(struct cmf *cmf) {

    if (!cmf)
        return 0;

    free_nul(cmf->offset);
    fclose_nul(cmf->fp);
    free_nul(cmf);

    return 0;
}

BGEN_API int bgen_create_metafile(struct bgen_file *bgen, const char *filepath,
                                  int verbose) {

    if (!bgen) {
        error("bgen_file pointer cannot be null.");
        return 1;
    }

    if (!filepath) {
        error("filepath pointer cannot be null.");
        return 1;
    }

    struct cmf *cmf = create_metafile(filepath, bgen_nvariants(bgen), 2);
    if (!cmf)
        return 1;

    struct bgen_cmf_next next_ctx;
    next_ctx.bgen = bgen;
    next_ctx.nvariants = bgen_nvariants(bgen);

    bopen_or_leave(bgen);

    if (fseek(bgen->file, bgen->variants_start, SEEK_SET)) {
        perror("Could not jump to variants start ");
        fclose(bgen->file);
        goto err;
    }

    if (write_metafile(cmf, &bgen_next_variant_metadata, &next_ctx, verbose))
        goto err;

    if (close_metafile(cmf))
        goto err;

    fclose(bgen->file);
    return 0;
err:
    fclose_nul(bgen->file);
    close_metafile(cmf);
    return 1;
}

struct bgen_idx *bgen_open_metafile(const char *filepath) {

    FILE *fp = NULL;
    struct bgen_idx *vm_idx = dalloc(sizeof(struct bgen_idx));
    if (!vm_idx)
        goto err;

    vm_idx->filepath = strdup(filepath);

    if (!(fp = fopen(vm_idx->filepath, "rb"))) {
        perror_fmt("Could not open %s", vm_idx->filepath);
        goto err;
    }

    char header[13];
    if (fread1(header, 13 * sizeof(char), fp))
        goto err;

    if (strncmp(header, "bgen index 03", 13)) {
        error("Unrecognized bgen index version: %.13s.", header);
        goto err;
    }

    if (fread1(&(vm_idx->nvariants), sizeof(uint32_t), fp))
        goto err;

    if (fread1(&(vm_idx->metasize), sizeof(uint64_t), fp))
        goto err;

    if (fseek(fp, vm_idx->metasize, SEEK_CUR)) {
        error("Could to fseek to the number of partitions.");
        goto err;
    }

    if (fread1(&(vm_idx->npartitions), sizeof(uint32_t), fp))
        goto err;

    vm_idx->ppos = dalloc(sizeof(uint64_t));

    for (size_t i = 0; i < vm_idx->npartitions; ++i) {
        if (fread1(vm_idx->ppos + i, sizeof(uint64_t), fp))
            goto err;
    }

    return vm_idx;

err:
    if (vm_idx)
        free_nul(vm_idx->filepath);
    free_nul(vm_idx);
    fclose_nul(fp);
    return NULL;
}

int bgen_close_metafile(struct bgen_idx *vm_idx) {

    if (vm_idx == NULL)
        return 0;

    if (vm_idx->filepath != NULL)
        free(vm_idx->filepath);

    return 0;
}

BGEN_API int bgen_metafile_nparts(struct bgen_idx *v) { return v->npartitions; }
BGEN_API int bgen_metafile_nvars(struct bgen_idx *v) { return v->nvariants; }

BGEN_API struct bgen_vm *bgen_read_metavars(struct bgen_idx *v, int part, int *nvars) {
    struct bgen_vm *vars = NULL;
    FILE *fp = NULL;

    if (part >= v->npartitions) {
        error("The provided partition number is out-of-range.");
        goto err;
    }

    int chunk = v->nvariants / v->npartitions;
    *nvars = MIN(chunk, v->nvariants - chunk * part);
    vars = malloc((*nvars) * sizeof(struct bgen_vm));
    int i, j;
    for (i = 0; i < *nvars; ++i) {
        vars[i].id.str = NULL;
        vars[i].rsid.str = NULL;
        vars[i].chrom.str = NULL;
        vars[i].allele_ids = NULL;
    }

    if ((fp = fopen(v->filepath, "rb")) == NULL) {
        perror("Could not open bgen index file ");
        goto err;
    }

    if (fseek(fp, 13 + 4 + 8, SEEK_SET)) {
        perror("Could not fseek bgen index file ");
        goto err;
    }

    if (fseek(fp, v->ppos[part], SEEK_CUR)) {
        perror("Could not fseek bgen index file ");
        goto err;
    }

    for (i = 0; i < *nvars; ++i) {
        fread_int(fp, &vars[i].vaddr, 8);

        fread_str(fp, &vars[i].id, 2);
        fread_str(fp, &vars[i].rsid, 2);
        fread_str(fp, &vars[i].chrom, 2);

        fread_int(fp, &vars[i].position, 4);
        fread_int(fp, &vars[i].nalleles, 2);
        vars[i].allele_ids = malloc(sizeof(struct bgen_str) * vars[i].nalleles);

        for (j = 0; j < vars[i].nalleles; ++j) {
            fread_str(fp, vars[i].allele_ids + j, 4);
        }
    }

    return vars;
err:
    if (vars != NULL) {
        for (i = 0; i < *nvars; ++i) {

            if (vars[i].id.str != NULL)
                free(vars[i].id.str);

            if (vars[i].rsid.str != NULL)
                free(vars[i].rsid.str);

            if (vars[i].chrom.str != NULL)
                free(vars[i].chrom.str);

            if (vars[i].allele_ids != NULL) {
                for (j = 0; j < vars[i].nalleles; ++j) {
                    if (vars[i].allele_ids[j].str != NULL)
                        free(vars[i].allele_ids[j].str);
                }
                free(vars[i].allele_ids);
            }
        }
        free(vars);
    }
    if (fp != NULL)
        fclose(fp);
    return NULL;
}

BGEN_API void bgen_free_metavars(struct bgen_vm *vm, int nvars) {}
