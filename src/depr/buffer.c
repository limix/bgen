#include "buffer.h"
#include "athr.h"
#include "min.h"
#include "zip/zstd.h"
#include <stdio.h>

struct Buffer
{
    void *base;
    size_t size;
};

struct Buffer *buffer_create()
{
    struct Buffer *b = malloc(sizeof(struct Buffer));
    b->base = NULL;
    b->size = 0;
    return b;
}

int buffer_append(struct Buffer *b, void *mem, size_t size)
{
    b->base = realloc(b->base, b->size + size);
    memcpy((char *)b->base + b->size, mem, size);
    b->size += size;
    return 0;
}

void *buffer_base(struct Buffer *b) { return b->base; }

int buffer_destroy(struct Buffer *b)
{
    if (b) {
        if (b->base)
            free(b->base);
        b->size = 0;
        free(b);
    }
    return 0;
}

int buffer_store(const char *fp, struct Buffer *b)
{
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
        fclose(f);
        return 1;
    }

    dst = bgen_zstd(b->base, b->size, &dst_size);
    size64 = dst_size;
    if (fwrite(&size64, 1, 8, f) < 8) {
        perror("Error while writing buffer to file");
        fclose(f);
        return 1;
    }

    if (fwrite(dst, 1, dst_size, f) < dst_size) {
        perror("Error while writing buffer to file");
        free(dst);
        fclose(f);
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

int buffer_load(const char *fp, struct Buffer *b, int verbose)
{
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

    if (uncompressed_size == 0) {
        fprintf(stderr, "Invalid uncompressed size: 0\n");
        goto err;
    }

    b->size = (size_t)uncompressed_size;

    if (b->size == 0) {
        fprintf(stderr, "Invalid buffer size: 0\n");
        goto err;
    }

    b->base = malloc(b->size);
    if (b->base == NULL) {
        fprintf(stderr, "Error: file %s might be invalid.\n", fp);
        goto err;
    }

    if (fread(&compressed_size, 1, 8, f) != 8) {
        perror("Error while reading buffer size");
        goto err;
    }

    compressed = malloc((size_t)compressed_size);
    if (compressed == NULL) {
        fprintf(stderr, "Error: file %s might be invalid.\n", fp);
        goto err;
    }

    if (buffer_load_loop(f, compressed, (size_t)compressed_size, verbose)) {
        goto err;
    }

    dst_size = b->size;
    if (bgen_unzstd(compressed, (size_t)compressed_size, &(b->base), &dst_size)) {
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

inline static int ceildiv(size_t x, size_t y) { return (x + (y - 1)) / y; }

int buffer_load_loop(FILE *file, char *data, size_t size, int verbose)
{
    size_t chunk_size = 5242880;
    char *last, *current;
    struct athr *athr;
    int nsteps = ceildiv(size, chunk_size);

    current = data;
    last = data + size;

    chunk_size = smin(size, chunk_size);
    if (verbose) {
        athr = athr_create(nsteps, "Loading index");
    } else {
        athr = NULL;
    }

    while (current < last) {

        size_t read_size = smin(chunk_size, (size_t)(last - current));
        if (fread(current, 1, read_size, file) < read_size) {
            perror("Could not read compressed data");
            if (verbose)
                athr_finish(athr);
            return 1;
        }
        current += read_size;

        if (verbose)
            athr_consume(athr, 1);
    }

    if (verbose)
        athr_finish(athr);
    return 0;
}
