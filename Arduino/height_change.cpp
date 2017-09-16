#include "height_change.h"

HeightChange::HeightChange(double z)
{
    _z = z;
    _processed = false;
}

double HeightChange::z()
{
    return _z;
}

bool HeightChange::processed()
{
    return _processed;
}

void HeightChange::processed(bool p)
{
    _processed = p;
}
