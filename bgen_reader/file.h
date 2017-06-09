#ifndef FILE_H
#define FILE_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "bgen_reader.h"

int64_t bgen_fopen(BGenFile *bgenfile);
int64_t bgen_fclose(BGenFile *bgenfile);

int64_t fread_check(BGenFile      *bgenfile,
                    void *restrict buffer,
                    size_t         size);

#endif /* end of include guard: FILE_H */
