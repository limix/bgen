#include "endian.h"

int is_little_endian(void) {
    int num = 1;

    return *(char *)&num == 1;
}
