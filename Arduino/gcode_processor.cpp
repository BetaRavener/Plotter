#include "gcode_processor.h"
#include "Arduino.h"

void GCodeProcessor::process(const String& str)
{
    _command_length = 0;
    uint8_t i = 0, j = 0;
    GCodeToken token;

    uint8_t state = 0;
    const char * cstr = str.c_str();
    while (true)
    {
        uint8_t c = str.c_str()[i];

        if (state == 0)
        {
            i++;
            if (c >= 'A' && c <= 'Z')
            {
                token.code = c;
                state = 1;
                j = i;
            }
        }
        else if (state == 1)
        {
            if (!((c >= '0' && c <= '9') || c == '.' || c== '-'))
            {
                const char* begin = str.c_str() + j;
                if (token.code == 'G')
                    token.value.i = strtol(begin, nullptr, 10);
                else
                    token.value.d = strtod(begin, nullptr);
                _command_tokens[_command_length++] = token;
                state = 0;
            }
            else
                i++;
        }   

        if (c == '\0')
            break;
    }
}

int16_t GCodeProcessor::token_idx(uint8_t t)
{
    for (int i = 0; i < _command_length; i++)
        if (_command_tokens[i].code == t)
            return i;
    return -1;
}

int32_t GCodeProcessor::token_int(uint8_t idx)
{
    return _command_tokens[idx].value.i;
}

double GCodeProcessor::token_double(uint8_t idx)
{
    return _command_tokens[idx].value.d;
}
