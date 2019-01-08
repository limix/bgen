#ifndef CASS_H
#define CASS_H

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define cass_message(file, line) \
    fprintf(stderr, "Assertion error at %s:%d\n", file, line);

static inline int _same_kind(double a, double b)
{
    return isnan(a) == isnan(b);
}

#define assert_equal_int(a, d)                           \
    {                                                    \
        double _a = a;                                   \
        double _d = d;                                   \
        if (!(_a == _d)) {                               \
            cass_message(__FILE__, __LINE__);            \
            fprintf(stderr, " Items are not equal:\n");  \
            fprintf(stderr, "  ACTUAL: %d\n", (int)_a);  \
            fprintf(stderr, "  DESIRED: %d\n", (int)_d); \
            return 1;                                    \
        }                                                \
    }

static inline int _almost_equal(double a, double d)
{
    return !_same_kind(a, d) || (fabs(a - d) > 1e-7);
}
#define assert_almost_equal(a, d)                              \
    {                                                          \
        double _a = a;                                         \
        double _d = d;                                         \
        if (_almost_equal(_a, _d)) {                           \
            cass_message(__FILE__, __LINE__);                  \
            fprintf(stderr, " Items are not close:\n");        \
            fprintf(stderr, "  ACTUAL: %.10f\n", (double)_a);  \
            fprintf(stderr, "  DESIRED: %.10f\n", (double)_d); \
            return 1;                                          \
        }                                                      \
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
