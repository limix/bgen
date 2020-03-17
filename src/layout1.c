#include "layout1.h"
#include "bgen/bgen.h"
#include "file.h"
#include "free.h"
#include "genotype.h"
#include "mem.h"
#include "report.h"
#include "zip/zlib.h"
#include <math.h>
#include <stdlib.h>

static void  read_unphased(struct bgen_genotype* vg, double* probabilities);
static char* decompress(struct bgen_file* bgen_file);

int bgen_layout1_read_header(struct bgen_file* bgen_file, struct bgen_genotype* genotype)
{
    char* chunk = NULL;

    if (bgen_file_compression(bgen_file) > 0) {
        chunk = decompress(bgen_file);
    } else {
        size_t size = 6 * bgen_file_nsamples(bgen_file);
        chunk = malloc(size);

        FILE* stream = bgen_file_stream(bgen_file);
        if (fread(chunk, size, 1, stream) < 1) {
            bgen_free(chunk);
            return 1;
        }
    }

    genotype->nsamples = bgen_file_nsamples(bgen_file);
    genotype->nalleles = 2;
    genotype->ncombs = 3;
    genotype->min_ploidy = 2;
    genotype->max_ploidy = 2;
    genotype->chunk = chunk;
    genotype->chunk_ptr = chunk;

    return 0;
}

void bgen_layout1_read_genotype(struct bgen_genotype* genotype, double* probabilities)
{
    read_unphased(genotype, probabilities);
}

static void read_unphased(struct bgen_genotype* genotype, double* probabilities)
{
    uint16_t ui_prob = 0;
    double   denom = 32768;

    char const* restrict chunk = genotype->chunk_ptr;

    for (uint32_t j = 0; j < genotype->nsamples; ++j) {
        unsigned int ui_prob_sum = 0;

        for (size_t i = 0; i < 3; ++i) {
            bgen_memfread(&ui_prob, &chunk, 2);
            probabilities[j * 3 + i] = ui_prob / denom;
            ui_prob_sum += ui_prob;
        }

        if (ui_prob_sum == 0) {
            for (size_t i = 0; i < 3; ++i)
                probabilities[j * 3 + i] = NAN;
        }
    }
}

static char* decompress(struct bgen_file* bgen_file)
{
    uint32_t compressed_length = 0;
    FILE*    stream = bgen_file_stream(bgen_file);

    if (fread(&compressed_length, sizeof(compressed_length), 1, stream) < 1) {
        bgen_perror_eof(stream, "could not read chunk size");
        return NULL;
    }

    char* compressed_chunk = malloc(compressed_length);

    if (fread(compressed_chunk, compressed_length, 1, stream) < 1) {
        bgen_perror_eof(stream, "could not read compressed chunk");
        bgen_free(compressed_chunk);
        return NULL;
    }

    if (bgen_file_compression(bgen_file) != 1) {
        bgen_error("compression flag should be 1; not %u", bgen_file_compression(bgen_file));
        bgen_free(compressed_chunk);
        return NULL;
    }

    size_t length = 10 * compressed_length;
    char*  chunk = malloc(length);

    bgen_unzlib_chunked(compressed_chunk, compressed_length, &chunk, &length);

    bgen_free(compressed_chunk);

    return chunk;
}
