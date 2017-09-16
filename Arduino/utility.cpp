#include "utility.h"

void swap(int32_t *a, int32_t *b)
{
    int32_t t = *a;
    *a = *b;
    *b = t;
}

uint32_t time_diff(uint32_t t2, uint32_t t1)
{
    return t2 >= t1 ? t2 - t1 : t1 - t2;
}