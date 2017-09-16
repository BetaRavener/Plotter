#ifndef PCF_H
#define PCF_H

#include <stdint.h>

class PCF
{
public:
    PCF(uint8_t address = 32);
    void write(uint8_t byte);
private:
    int _address;
};

#endif