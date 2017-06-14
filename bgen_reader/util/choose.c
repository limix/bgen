#include "choose.h"

inti bgen_reader_choose(inti n, inti k)
{
    inti ans = 1;

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
