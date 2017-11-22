#include "progressbar/progressbar.h"
#include "util/bits.h"
#include "util/buffer.h"
#include "zip/zstd_wrapper.h"
#include <stdio.h>

struct Buffer {
    void *base;
    size_t size;
};

struct Buffer *buffer_create() {
    struct Buffer *b = malloc(sizeof(struct Buffer));
    b->base = NULL;
    b->size = 0;
    return b;
}

int buffer_append(struct Buffer *b, void *mem, size_t size) {
    b->base = realloc(b->base, b->size + size);
    memcpy(b->base + b->size, mem, size);
    b->size += size;
    return 0;
}

void *buffer_base(struct Buffer *b) { return b->base; }

size_t buffer_size(struct Buffer *b) { return b->size; }

int buffer_destroy(struct Buffer *b) {
    free(b->base);
    b->size = 0;
    free(b);
    return 0;
}

int buffer_store(const char *fp, struct Buffer *b) {
    FILE *f;
    uint64_t size64;
    size_t dst_size;
    void *dst;

    if ((f = fopen(fp, "wb")) == NULL) {
        perror("Error while storing buffer");
        return 1;
    }

    size64 = b->size;
    if (fwrite(&size64, 1, 8, f) < 8) {
        perror("Error while writing buffer to file");
        return 1;
    }

    dst = bgen_zstd(b->base, b->size, &dst_size);
    size64 = dst_size;
    if (fwrite(&size64, 1, 8, f) < 8) {
        perror("Error while writing buffer to file");
        return 1;
    }

    if (fwrite(dst, 1, dst_size, f) < dst_size) {
        perror("Error while writing buffer to file");
        free(dst);
        return 1;
    }

    if (fclose(f)) {
        perror("Error while closing file for storing buffer");
        free(dst);
        return 1;
    }

    free(dst);
    return 0;
}

int buffer_load_loop(FILE *file, char *data, size_t size, int verbose);

int buffer_load(const char *fp, struct Buffer *b, int verbose) {
    FILE *f;
    uint64_t uncompressed_size, compressed_size;
    size_t dst_size;
    char *compressed;

    f = NULL;
    b->base = NULL;
    compressed = NULL;

    if ((f = fopen(fp, "rb")) == NULL) {
        perror("Error while opening buffer file");
        goto err;
    }

    if (fread(&uncompressed_size, 1, 8, f) != 8) {
        perror("Error while reading buffer size");
        goto err;
    }

    b->size = uncompressed_size;
    b->base = malloc(b->size);

    if (fread(&compressed_size, 1, 8, f) != 8) {
        perror("Error while reading buffer size");
        goto err;
    }

    compressed = malloc(compressed_size);
    if (buffer_load_loop(f, compressed, compressed_size, verbose)) {
        goto err;
    }

    dst_size = b->size;
    if (bgen_unzstd(compressed, compressed_size, &(b->base), &dst_size)) {
        goto err;
    }

    if (fclose(f)) {
        perror("Error while closing file for loading buffer");
        f = NULL;
        goto err;
    }

    free(compressed);
    return 0;

err:
    if (compressed != NULL)
        free(compressed);
    if (b->base != NULL)
        free(b->base);
    if (f != NULL)
        fclose(f);
    return 1;
}

int buffer_load_loop(FILE *file, char *data, size_t size, int verbose)
{
    size_t chunk_size = 5242880;
    size_t read_size;
    char *last, *current;
    progressbar *progress;
    int nsteps = CEILDIV(size, chunk_size);

    current = data;
    last = data + size;

    chunk_size = MIN(size, chunk_size);
    if (verbose)
        progress = progressbar_new("Loading index", nsteps);
    else
        progress = NULL;

    while (current < last) {

        read_size = MIN(chunk_size, (size_t) (last - current));
        if (fread(current, 1, read_size, file) < read_size) {
            perror("Could not read compressed data");
            if (verbose)
                progressbar_finish(progress);
            return 1;
        }
        current += read_size;

        if (verbose)
            progressbar_inc(progress);
    }

    if (verbose)
        progressbar_finish(progress);
    return 0;
}