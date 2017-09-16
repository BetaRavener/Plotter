#ifndef CIRCULAR_STRING_BUFFER
#define CIRCULAR_STRING_BUFFER

#include <stdint.h>
#include <WString.h>

#define CIRCULAR_BUFFER_SIZE 400

class CircularStringBuffer
{
public:
    bool get(char& c);
    bool get_line(String& str);
    bool put(char str);
    void clear();

    bool is_full();
    bool is_empty();

private:
    char _storage[CIRCULAR_BUFFER_SIZE];
    // Index from which to take next item
    int _get_idx;
    // Index where next item will be placed
    int _put_idx;
};

#endif
