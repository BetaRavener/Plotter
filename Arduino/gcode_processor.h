#ifndef GCODE_PROCESSOR
#define GCODE_PROCESSOR

#include <stdint.h>
#include <WString.h>
#include "gcode_token.h" 

#define MAX_TOKENS_PER_COMMAND 10

class GCodeProcessor
{
public:
    void process(const String& str);
    int16_t token_idx(uint8_t t);
    int32_t token_int(uint8_t idx);
    double token_double(uint8_t idx);
private:
    GCodeToken _command_tokens[MAX_TOKENS_PER_COMMAND];
    uint8_t _command_length;
};

#endif // !GCODE_PROCESSOR
