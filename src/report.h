#ifndef BGEN_REPORT_H
#define BGEN_REPORT_H

#include <stdarg.h>
#include <stdio.h>

#if defined(HAVE_ATTR_FORMAT)
#define ATTR_FORMAT12 __attribute__((format(printf, 1, 2)))
#define ATTR_FORMAT23 __attribute__((format(printf, 2, 3)))
#else
#define ATTR_FORMAT12
#define ATTR_FORMAT23
#endif

void bgen_warning(char const* err, ...) ATTR_FORMAT12;
void bgen_error(char const* err, ...) ATTR_FORMAT12;
void bgen_perror(char const* err, ...) ATTR_FORMAT12;
void bgen_perror_eof(FILE* stream, char const* err, ...) ATTR_FORMAT23;
void bgen_die(char const* err, ...) ATTR_FORMAT12;

#endif
