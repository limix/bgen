#ifndef CASS_H
#define CASS_H

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define cass_message(file, line) \
    fprintf(stderr, "Assertion error at %s:%d\n", file, line);

#define assert_equal_int(a, d)                      \
    if (!(a == d)) {                                \
        cass_message(__FILE__, __LINE__);           \
        fprintf(stderr, " Items are not equal:\n"); \
        fprintf(stderr, "  ACTUAL: %d\n", (int)a);  \
        fprintf(stderr, "  DESIRED: %d\n", (int)d); \
        return 1;                                   \
    }

#define assert_almost_equal(a, d)                      \
    if (fabs(a - d) > 1e-7) {                          \
        cass_message(__FILE__, __LINE__);              \
        fprintf(stderr, " Items are not close:\n");    \
        fprintf(stderr, "  ACTUAL: %f\n", (double)a);  \
        fprintf(stderr, "  DESIRED: %f\n", (double)d); \
        return 1;                                      \
    }

#define assert_equal_uint64(a, d)                                 \
    if (!(a == d)) {                                              \
        cass_message(__FILE__, __LINE__);                         \
        fprintf(stderr, " Items are not equal:\n");               \
        fprintf(stderr, "  ACTUAL: %" PRIu64 "\n", (uint64_t)a);  \
        fprintf(stderr, "  DESIRED: %" PRIu64 "\n", (uint64_t)d); \
        return 1;                                                 \
    }

#define assert_not_equal_int(a, d)                      \
    if (!(a != d)) {                                    \
        cass_message(__FILE__, __LINE__);               \
        fprintf(stderr, " Items are not different:\n"); \
        fprintf(stderr, "  ACTUAL: %d\n", (int)a);      \
        fprintf(stderr, "  UNDESIRED: %d\n", (int)d);   \
        return 1;                                       \
    }

#define assert_null(a)                                     \
    if ((a) != NULL) {                                     \
        cass_message(__FILE__, __LINE__);                  \
        fprintf(stderr, " Address should not be NULL:\n"); \
        fprintf(stderr, "  EXPRESSION: " #a "\n");         \
        return 1;                                          \
    }

#define assert_not_null(a)                                 \
    if ((a) == NULL) {                                     \
        cass_message(__FILE__, __LINE__);                  \
        fprintf(stderr, " Address should not be NULL:\n"); \
        fprintf(stderr, "  EXPRESSION: " #a "\n");         \
        return 1;                                          \
    }

#define assert_strncmp(a, d, len)                     \
    if (strncmp(a, d, len) != 0) {                    \
        cass_message(__FILE__, __LINE__);             \
        fprintf(stderr, " Items are not equal:\n");   \
        fprintf(stderr, "  ACTUAL: %.*s\n", len, a);  \
        fprintf(stderr, "  DESIRED: %.*s\n", len, d); \
        return 1;                                     \
    }

#endif
