#include "choose.h"

inti bgen_choose(inti n, inti k)
{
    inti ans;
    int j;

    ans = 1;

    k = k > n - k ? n - k : k;
    j = 1;

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
