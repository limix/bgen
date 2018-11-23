#include "endian.h"

int bgen_is_lendian(void) {
    int num = 1;

    return *(char *)&num == 1;
}
