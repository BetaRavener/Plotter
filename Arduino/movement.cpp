#include "movement.h"

Movement::Movement(int32_t x, int32_t y)
{
    _x = x;
    _y = y;
}

int32_t Movement::x()
{
    return _x;
}

int32_t Movement::y()
{
    return _y;
}