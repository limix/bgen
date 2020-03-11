#ifndef BGEN_FILE_H_PRIVATE
#define BGEN_FILE_H_PRIVATE

#include <stdio.h>

struct bgen_file;

FILE*       bgen_file_stream(struct bgen_file const* bgen_file);
char const* bgen_file_filepath(struct bgen_file const* bgen_file);
unsigned    bgen_file_layout(struct bgen_file const* bgen_file);
unsigned    bgen_file_compression(struct bgen_file const* bgen_file);
int         bgen_file_seek_variants_start(struct bgen_file* bgen_file);

#endif
