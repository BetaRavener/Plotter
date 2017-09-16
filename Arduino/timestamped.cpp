#include "timestamped.h"

Timestamped::Timestamped() :
    _timestamp(0)
{
}

uint32_t Timestamped::timestamp()
{
    return _timestamp;
}

void Timestamped::timestamp(uint32_t t)
{
    _timestamp = t;
}

