#include "gcode_io.h"
#include <Arduino.h>
#include "utility.h"

GCodeIO::GCodeIO()
{
}

void GCodeIO::setup()
{
    Serial.begin(57600);
    Serial.setTimeout(0);
}

void GCodeIO::restart()
{
    _received_lines = 0;
    _confirmed_lines = 0;
    _serial_input.clear();
}

bool GCodeIO::check_new_request()
{
    String str;

    if (!_try_read_line(str))
        return false;

    if (str == "new")
    {
        restart();
        Serial.println("ready");
        return true;
    }

    return false;
}

String GCodeIO::receive(uint32_t timeout_ms)
{
    String input;
    uint32_t start = millis();
    while (time_diff(millis(), start) < timeout_ms)
    {
        bool error = false;
        if (!_try_read_line(input))
            continue;
        else if (input == "end")
        {
            return "";
        }
        else if (input[0] == '!')
            continue;

        int ch_idx = input.lastIndexOf('*');
        error = ch_idx == -1;
        const char *begin;
        char *end;
        if (!error)
        {
            begin = input.c_str() + (ch_idx + 1);
            uint8_t csum = strtol(begin, &end, 10);
            error = end == begin || csum != _checksum(input.substring(0, ch_idx));
        }

        int ln_idx = 0;
        if (!error)
        {
            ln_idx = input.indexOf('*');
            error = ln_idx == -1;
        }

        int32_t line_number = 0;
        if (!error)
        {
            begin = input.c_str();
            line_number = strtol(begin, &end, 10);
            error = end != (begin + ln_idx);
        }

        if (!error)
        {
            if (line_number < _received_lines)
                continue;
            else if (line_number > _received_lines)
            {
                error = true;
            }
            else
            {
                _received_lines++;
                String str(_confirmed_lines++);
                str += "*ok";
                send(str);
                return input.substring(ln_idx + 1, ch_idx);
            }
        }

        if (error)
        {
            String str(_received_lines);
            str += "*resend";
            send(str);
            Serial.print("!");
            Serial.println(input);
            _serial_input.clear();
        }
    }
    return "";
}

void GCodeIO::send(const String& str)
{
    uint8_t csum = _checksum(str);
    String output(str);
    output += "*";
    output += csum;
    Serial.println(output);
}

void GCodeIO::buffer_incoming()
{
    int c;
    while ((c = Serial.read()) > 0) {
        _serial_input.put(c);
    }
}

bool GCodeIO::_try_read_line(String& str)
{
    buffer_incoming();
    return _serial_input.get_line(str);
}

uint8_t GCodeIO::_checksum(const String& str)
{
    uint8_t csum = 0;
    for (unsigned int i = 0; i < str.length(); i++)
        csum = csum ^ str[i];
    return csum;
}
