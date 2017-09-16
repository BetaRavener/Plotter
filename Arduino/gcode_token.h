#ifndef GCODE_TOKEN
#define GCODE_TOKEN

#include <stdint.h>

class GCodeToken
{
public:
    uint8_t code;
    union Value {
        int32_t i;
        double d;
    } value;
};

#endif