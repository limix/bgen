#ifndef BGEN_REPORT_H
#define BGEN_REPORT_H

#include "imm/api.h"
#include <stdarg.h>

#if defined(HAVE_ATTR_FORMAT)
#define ATTR_FORMAT __attribute__((format(printf, 1, 2)))
#else
#define ATTR_FORMAT
#endif

void bgen_warning(char const* err, ...) ATTR_FORMAT;
void bgen_error(char const* err, ...) ATTR_FORMAT;
void bgen_perror(char const* err, ...) ATTR_FORMAT;
void bgen_die(char const* err, ...) ATTR_FORMAT;

#endif
