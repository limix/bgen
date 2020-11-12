#include "report.h"
#include <errno.h>
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

void bgen_perror(char const* err, ...)
{
    va_list params;
    va_start(params, err);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, err, params);
    fprintf(stderr, " (%s)", strerror(errno));
    fputc('\n', stderr);
    va_end(params);
}

void bgen_perror_eof(FILE* stream, char const* err, ...)
{
    va_list params;
    va_start(params, err);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, err, params);
    if (feof(stream))
        fprintf(stderr, " (%s)", "unexpected end of file");
    else
        fprintf(stderr, " (%s)", strerror(errno));
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
