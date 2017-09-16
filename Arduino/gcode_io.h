#ifndef GCODE_IO_H
#define GCODE_IO_H

#include <stdint.h>
#include <WString.h>

#include "circular_string_buffer.h"

class GCodeIO
{
public:
    GCodeIO();
    void setup();
    void restart();
    bool check_new_request();
    String receive(uint32_t timeout_ms);
    void send(const String& str);
    void buffer_incoming();
private:
    bool _try_read_line(String& str);
    uint8_t _checksum(const String& str);
    
    int _received_lines;
    int _confirmed_lines;
    bool _recv_error;
    bool _error_reported;
    CircularStringBuffer _serial_input;
};

#endif