#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdint.h>

int64_t fread_check(void *restrict buffer,
                    size_t         size,
                    FILE *restrict stream,
                    char          *filepath);

#endif /* end of include guard: FILE_H */
