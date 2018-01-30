#include "tool.h"
#include "Arduino.h"

Tool::Tool(int pin, int min_height, int max_height)
{
    _control_pin = pin;
    _state = UP;
}

void Tool::setup()
{
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

void Tool::_update_output()
{
	if (_state == UP)
		digitalWrite(_control_pin, LOW);
	else if (_state == DOWN)
		digitalWrite(_control_pin, HIGH);
}
