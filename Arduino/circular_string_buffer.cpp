#include "circular_string_buffer.h"

#include "Arduino.h"

bool CircularStringBuffer::get(char& c)
{
    if (is_empty())
        return false;

    c = _storage[_get_idx];
    _get_idx = (_get_idx + 1) % CIRCULAR_BUFFER_SIZE;
    return true;
}

bool CircularStringBuffer::get_line(String& str)
{
    if (is_empty())
        return false;

    int ln_idx = _get_idx;
    int cnt = 0;
    // Search for newline character
    while (true)
    {
        if (ln_idx == _put_idx)
            return false;
        if (_storage[ln_idx] == '\n')
            break;
        ln_idx = (ln_idx + 1) % CIRCULAR_BUFFER_SIZE;
        cnt++;
    }

    str.reserve(cnt);
    str = "";
    while (_get_idx != ln_idx)
    {
        //if (!str.append(_storage[_get_idx]))
        //    break;
        str += _storage[_get_idx];
        _get_idx = (_get_idx + 1) % CIRCULAR_BUFFER_SIZE;
    }

    // Skip also newline character
    _get_idx = (ln_idx + 1) % CIRCULAR_BUFFER_SIZE;

    return true;
}

bool CircularStringBuffer::put(char str)
{
    if (is_full())
        return false;

    _storage[_put_idx] = str;
    _put_idx = (_put_idx + 1) % CIRCULAR_BUFFER_SIZE;
    return true;
}

void CircularStringBuffer::clear()
{
    _get_idx = _put_idx = 0;
}

bool CircularStringBuffer::is_full()
{
    return (_get_idx + CIRCULAR_BUFFER_SIZE - _put_idx) % CIRCULAR_BUFFER_SIZE == 1;
}

bool CircularStringBuffer::is_empty()
{
    return _get_idx == _put_idx;
}
