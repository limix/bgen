#include "util/buffer.h"
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

inti buffer_append(struct Buffer *b, void *mem, size_t size) {
    b->base = realloc(b->base, b->size + size);
    memcpy(b->base + b->size, mem, size);
    b->size += size;
    return 0;
}

void *buffer_base(struct Buffer *b) { return b->base; }

size_t buffer_size(struct Buffer *b) { return b->size; }

inti buffer_destroy(struct Buffer *b) {
    free(b->base);
    b->size = 0;
    free(b);
    return 0;
}

inti buffer_store(const char *fp, struct Buffer *b) {
    FILE *f;
    uint64_t size64;

    if ((f = fopen(fp, "wb")) == NULL) {
        fprintf(stderr, "Error while storing buffer");
        perror(fp);
        return 1;
    }

    size64 = b->size;
    if (fwrite(&size64, 1, 8, f) != 8) {
        fprintf(stderr, "Error while writing buffer to file");
        perror(fp);
        return 1;
    }

    if (fwrite(b->base, 1, b->size, f) != b->size) {
        fprintf(stderr, "Error while writing buffer to file");
        perror(fp);
        return 1;
    }

    if (fclose(f)) {
        fprintf(stderr, "Error while closing file for storing buffer");
        perror(fp);
        return 1;
    }
    // dst = bgen_zstd(buffer_base(b), buffer_size(b), &dst_size);

    return 0;
}

inti buffer_load(const char *fp, struct Buffer *b)
{
    FILE *f;
    uint64_t size64;
    size64 = -1;

    if ((f = fopen(fp, "rb")) == NULL) {
        fprintf(stderr, "Error while opening buffer file ");
        perror(fp);
        return 1;
    }

    if (fread(&size64, 1, 8, f) != 8) {
        fprintf(stderr, "Error while reading buffer size of ");
        perror(fp);
        return 1;
    }

    b->size = size64;
    b->base = malloc(b->size);
    if (fread(b->base, 1, b->size, f) != b->size) {
        fprintf(stderr, "Error while reading buffer from file ");
        perror(fp);
        return 1;
    }

    if (fclose(f)) {
        fprintf(stderr, "Error while closing file for loading buffer from ");
        perror(fp);
        return 1;
    }

    return 0;
}
// inti bgen_unzstd(const byte *, inti, byte **, inti *);
// byte *bgen_zstd(const byte *src, inti src_size, inti *dst_size);
