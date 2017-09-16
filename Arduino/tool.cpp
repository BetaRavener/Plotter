#include "tool.h"
#include "Arduino.h"

Tool::Tool(int pin, int min_height, int max_height)
{
    _servo_pin = pin;
    _min_height = min_height;
    _max_height = max_height;
    _up_height = 180;
    _down_height = 130;
    _state = UP;
}

void Tool::setup()
{
    _servo.attach(_servo_pin, 800, 1500);
    _update_output();
}

Tool::State Tool::state()
{
    return _state;
}

void Tool::change_state(State state)
{
    _state = state;
    _update_output();
}

int Tool::height(State state)
{
    if (state == UP)
        return _up_height;
    else if (state == DOWN)
        return _down_height;
}

void Tool::change_height(State state, int val)
{
    val = val > _max_height ? _max_height : (val < _min_height ? _min_height : val);

    if (state == UP)
        _up_height = val;
    else if (state == DOWN)
        _down_height = val;
    _update_output();
}

void Tool::_update_output()
{
    if (_state == UP)
        _servo.write(_up_height);
    else if (_state == DOWN)
        _servo.write(_down_height);
}
