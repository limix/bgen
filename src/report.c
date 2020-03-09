#include "report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bgen_warning(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    fprintf(stderr, "WARNING: ");
    vfprintf(stderr, err, params);
    fputc('\n', stderr);
    va_end(params);
}

void bgen_error(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, err, params);
    fputc('\n', stderr);
    va_end(params);
}

void bgen_perror(FILE* stream, char const* err, ...)
{
    va_list params;
    va_start(params, err);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, err, params);
    fprintf(stderr, " (%s)", strerror(ferror(stream)));
    fputc('\n', stderr);
    va_end(params);
}

void bgen_die(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    bgen_error(err, params);
    va_end(params);
    exit(1);
}
