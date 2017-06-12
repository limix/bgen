#ifndef CHOOSE_H
#define CHOOSE_H

#include <stdint.h>

inline static uint64_t choose(uint64_t n, uint64_t k)
{
    uint64_t ans = 1;

    k = k > n - k ? n - k : k;
    int j = 1;

    for (; j <= k; j++, n--)
    {
        if (n % j == 0)
        {
            ans *= n / j;
        } else
        if (ans % j == 0)
        {
            ans = ans / j * n;
        } else
        {
            ans = (ans * n) / j;
        }
    }
    return ans;
}

#endif /* end of include guard: CHOOSE_H */
