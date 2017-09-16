#ifndef TIMESTAMPED_H
#define TIMESTAMPED_H

#include <stdint.h>

class Timestamped
{
public:
    Timestamped();
    uint32_t timestamp();
    void timestamp(uint32_t t);

protected:
    uint32_t _timestamp;
};

#endif // !TIMESTAMPED_H
