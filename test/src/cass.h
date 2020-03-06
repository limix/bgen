#ifndef CASS_H
#define CASS_H

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int cass_errors = 0;

inline static int cass_close_impl(double actual, double desired,
                                  double rel_tol, double abs_tol);

inline static void cass_print_context(const char *filename, int line)
{
    fprintf(stderr, "Assertion error at %s:%d\n", filename, line);
}

inline static int cass_status(void)
{
    if (cass_errors == 1)
        printf("Attention: %d assertion has failed.\n", cass_errors);
    if (cass_errors > 1)
        printf("Attention: %d assertions have failed.\n", cass_errors);
    return cass_errors;
}

#define cass_equal_int(a, d)                                                  \
    {                                                                         \
        double _a = a;                                                        \
        double _d = d;                                                        \
        if (!(_a == _d)) {                                                    \
            cass_print_context(__FILE__, __LINE__);                           \
            fprintf(stderr, " Items are not equal:\n");                       \
            fprintf(stderr, "  ACTUAL: %d\n", (int)_a);                       \
            fprintf(stderr, "  DESIRED: %d\n\n", (int)_d);                    \
            ++cass_errors;                                                    \
        }                                                                     \
    }

#define cass_close(actual, desired) cass_close2(actual, desired, 1e-09, 0.0)

#define cass_close2(actual, desired, rel_to, abs_tol)                         \
    {                                                                         \
        double _a = actual;                                                   \
        double _d = desired;                                                  \
        if (cass_close_impl(_a, _d, 1e-09, 0.0)) {                            \
            cass_print_context(__FILE__, __LINE__);                           \
            fprintf(stderr, " Items are not close:\n");                       \
            fprintf(stderr, "  ACTUAL : %.10f\n", (double)_a);                \
            fprintf(stderr, "  DESIRED: %.10f\n\n", (double)_d);              \
            ++cass_errors;                                                    \
        }                                                                     \
    }

#define cass_equal_uint64(a, d)                                               \
    if (!(a == d)) {                                                          \
        cass_print_context(__FILE__, __LINE__);                               \
        fprintf(stderr, " Items are not equal:\n");                           \
        fprintf(stderr, "  ACTUAL : %" PRIu64 "\n", (uint64_t)a);             \
        fprintf(stderr, "  DESIRED: %" PRIu64 "\n\n", (uint64_t)d);           \
        ++cass_errors;                                                        \
    }

#define cass_not_equal_int(a, d)                                              \
    if (!(a != d)) {                                                          \
        cass_print_context(__FILE__, __LINE__);                               \
        fprintf(stderr, " Items are not different:\n");                       \
        fprintf(stderr, "  ACTUAL   : %d\n", (int)a);                         \
        fprintf(stderr, "  UNDESIRED: %d\n\n", (int)d);                       \
        ++cass_errors;                                                        \
    }

#define cass_null(a)                                                          \
    if ((a) != NULL) {                                                        \
        cass_print_context(__FILE__, __LINE__);                               \
        fprintf(stderr, " Address should not be NULL:\n");                    \
        fprintf(stderr, "  EXPRESSION: " #a "\n\n");                          \
        ++cass_errors;                                                        \
    }

#define cass_not_null(a)                                                      \
    if ((a) == NULL) {                                                        \
        cass_print_context(__FILE__, __LINE__);                               \
        fprintf(stderr, " Address should not be NULL:\n");                    \
        fprintf(stderr, "  EXPRESSION: " #a "\n\n");                          \
        ++cass_errors;                                                        \
    }

#define cass_strncmp(a, d, len)                                               \
    if (strncmp(a, d, len) != 0) {                                            \
        cass_print_context(__FILE__, __LINE__);                               \
        fprintf(stderr, " Items are not equal:\n");                           \
        fprintf(stderr, "  ACTUAL : %.*s\n", len, a);                         \
        fprintf(stderr, "  DESIRED: %.*s\n\n", len, d);                       \
        ++cass_errors;                                                        \
    }

#define cass_cond(a)                                                          \
    if (!(a)) {                                                               \
        cass_print_context(__FILE__, __LINE__);                               \
        fprintf(stderr, " Condition evaluates to false:\n");                  \
        fprintf(stderr, "  EXPRESSION: " #a "\n\n");                          \
        ++cass_errors;                                                        \
    }

inline static int cass_close_impl(double actual, double desired,
                                  double rel_tol, double abs_tol)
{
    /* This implementation is basically a copy of the `math.isclose`
     * implementation of the Python library.
     */
    if (actual == desired) {
        /* short circuit exact equality -- needed to catch two infinities of
         * the same sign. And perhaps speeds things up a bit sometimes.
         */
        return 0;
    }

    /* This catches the case of two infinities of opposite sign, or
     * one infinity and one finite number. Two infinities of opposite
     * sign would otherwise have an infinite relative tolerance.
     * Two infinities of the same sign are caught by the equality check
     * above.
     */

    if (isinf(actual) || isinf(desired)) {
        return 1;
    }

    /* now do the regular computation
     * this is essentially the "weak" test from the Boost library
     */

    double diff = fabs(desired - actual);

    return !(((diff <= fabs(rel_tol * desired)) ||
              (diff <= fabs(rel_tol * actual))) ||
             (diff <= abs_tol));
}

#endif
