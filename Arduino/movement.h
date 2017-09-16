#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdint.h>

#include "timestamped.h"

class Movement : public Timestamped
{
public:
    Movement() {}
    Movement(int32_t x, int32_t y);

    int32_t x();
    int32_t y();

    
private:
    int32_t _x, _y;
};

#endif // !MOVEMENT_H

