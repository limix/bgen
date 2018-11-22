#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>

#define echo(...)                                                                       \
    do {                                                                                \
        printf(__VA_ARGS__);                                                            \
        puts("");                                                                       \
    } while (0)

#define error(...)                                                                      \
    do {                                                                                \
        fprintf(stderr, __VA_ARGS__);                                                   \
        fputs("", stderr);                                                              \
    } while (0)

#endif
