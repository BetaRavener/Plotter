#include "configuration.h"

void Configuration::process(Keypad& keypad, bool activity, MotorUnit& unit, Tool& tool)
{
    // Check buttons that react to presses
    bool left_pressed = false;
    bool right_pressed = false;
    bool up_pressed = false;
    bool down_pressed = false;
    if (activity)
    {
        int old_stage = _stage;
        if (keypad.isPressed('1'))
        {
            if (_stage > Z_UP)
                _stage--;
        }
        else if (keypad.isPressed('3'))
        {
            if (_stage < FINISHED)
                _stage++;
        }
        left_pressed = keypad.isPressed('4');
        right_pressed = keypad.isPressed('6');
        up_pressed = keypad.isPressed('2');
        down_pressed = keypad.isPressed('8');

        if (_stage != old_stage)
        {
            tool.change_state(_stage == Z_DOWN ? Tool::DOWN : Tool::UP);
            return;
        }
    }

    // Check buttons that react to hold
    int idx;
    if (up_pressed || ((idx = keypad.findInList('2')) != -1 && keypad.key[idx].kstate == HOLD))
    {
        switch (_stage)
        {
        case Z_UP:
            tool.change_height(Tool::UP, tool.height(Tool::UP) - 1);
            delay(50);
            break;
        case XY:
            unit.step_y(Motor::BACKWARD);
            delay(5);
            break;
        case Z_DOWN:
            tool.change_height(Tool::DOWN, tool.height(Tool::DOWN) - 1);
            delay(50);
            break;
        }
    }
    if (down_pressed || ((idx = keypad.findInList('8')) != -1 && keypad.key[idx].kstate == HOLD))
    {
        switch (_stage)
        {
        case Z_UP:
            tool.change_height(Tool::UP, tool.height(Tool::UP) + 1);
            delay(50);
            break;
        case XY:
            unit.step_y(Motor::FORWARD);
            delay(5);
            break;
        case Z_DOWN:
            tool.change_height(Tool::DOWN, tool.height(Tool::DOWN) + 1);
            delay(50);
            break;
        }
    }
    if (_stage == XY)
    {
        if (left_pressed || ((idx = keypad.findInList('4')) != -1 && keypad.key[idx].kstate == HOLD))
        {
            unit.step_x(Motor::BACKWARD);
            delay(5);
        }
        if (right_pressed || ((idx = keypad.findInList('6')) != -1 && keypad.key[idx].kstate == HOLD))
        {
            unit.step_x(Motor::FORWARD);
            delay(5);
        }
    }
}

bool Configuration::finished()
{
    return _stage == FINISHED;
}
