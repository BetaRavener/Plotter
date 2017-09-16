#include "tool_head.h"
#include "Arduino.h"
#include "utility.h"

#define MIN_MOTOR_DELAY_US 1300
#define MAX_MOTOR_DELAY_US 10000
#define MOTOR_DELAY_RANGE_US (MAX_MOTOR_DELAY_US - MIN_MOTOR_DELAY_US)
#define TOOL_DELAY_MS 1000

ToolHead::ToolHead(MotorUnit* motor_unit, Tool* tool) :
    _motor_unit(motor_unit),
    _tool(tool)
{
    _internal_resolution = min(_motor_unit->resolution_x(), _motor_unit->resolution_y());
    _head_x = 0;
    _head_y = 0;
}

Movement ToolHead::create_movement_to(double x_mm, double y_mm)
{
    int32_t x = _mm_to_internal(x_mm);
    int32_t y = _mm_to_internal(y_mm);
    return Movement(x, y);
}

DrawingLine ToolHead::create_line_to(double x_mm, double y_mm)
{
    int32_t x = _mm_to_internal(x_mm);
    int32_t y = _mm_to_internal(y_mm);
    return DrawingLine(_head_x, _head_y, x, y);
}

HeightChange ToolHead::create_height_change_to(double z_mm)
{
    return HeightChange(z_mm);
}

bool ToolHead::process(DrawingLine* line)
{
    uint32_t timestamp = micros();
    //TODO: Move return condition to main loop
    if (line->finished())
        return true;
        //return time_diff(timestamp, line->timestamp()) >= NEXT_COMMAND_DELAY_US;

    uint32_t delay = MAX_MOTOR_DELAY_US - (line->speed() * MOTOR_DELAY_RANGE_US) / 100;
    if (time_diff(timestamp, line->timestamp()) < delay)
        return false;

    line->timestamp(timestamp);

    line->step();
    if (line->finished())
        return false;

    _move_toward(line->x(), line->y());

    return false;
}

bool ToolHead::process(Movement* move)
{
    if (_head_x == move->x() && _head_y == move->y())
        return true;

    uint32_t timestamp = micros();
    if (time_diff(timestamp, move->timestamp()) < MIN_MOTOR_DELAY_US)
        return false;
    move->timestamp(timestamp);

    int32_t cx = 0, cy = 0;

    while (cx == 0 && _head_x != move->x())
    {
        _head_x += _head_x < move->x() ? 1 : -1;
        cx = _internal_to_real(_head_x, _motor_unit->resolution_x()) - _real_head_x();
    }

    while (cy == 0 && _head_y != move->y())
    {
        _head_y += _head_y < move->y() ? 1 : -1;
        cy = _internal_to_real(_head_y, _motor_unit->resolution_y()) - _real_head_y();
    }

    if (cx > 0)
        _motor_unit->step_x(Motor::FORWARD);
    else if (cx < 0)
        _motor_unit->step_x(Motor::BACKWARD);

    if (cy > 0)
        _motor_unit->step_y(Motor::FORWARD);
    else if (cy < 0)
        _motor_unit->step_y(Motor::BACKWARD);

    return false;
}

bool ToolHead::process(HeightChange* change)
{
    if (!change->processed())
    {
        if (change->z() < 0)
            _tool->change_state(Tool::DOWN);
        else
            _tool->change_state(Tool::UP);
        change->processed(true);
        change->timestamp(millis());
    }
    return millis() - change->timestamp() >= TOOL_DELAY_MS;
}

void ToolHead::idle()
{
    _motor_unit->stop();
}

bool ToolHead::is_idle()
{
    return _motor_unit->stopped();
}

bool ToolHead::_move_toward(int32_t x, int32_t y) {
    if (x > _head_x)
    {
        _head_x++;
    }
    else if (x < _head_x)
    {
        _head_x--;
    }

    if (y > _head_y)
    {
        _head_y++;
    }
    else if (y < _head_y)
    {
        _head_y--;
    }

    int32_t real_x = _internal_to_real(_head_x, _motor_unit->resolution_x());
    int32_t real_y = _internal_to_real(_head_y, _motor_unit->resolution_y());

    if (real_x > _real_head_x())
    {
        _motor_unit->step_x(Motor::FORWARD);
    }
    else if (real_x < _real_head_x())
    {
        _motor_unit->step_x(Motor::BACKWARD);
    }

    if (real_y > _real_head_y())
    {
        _motor_unit->step_y(Motor::FORWARD);
    }
    else if (real_y < _real_head_y())
    {
        _motor_unit->step_y(Motor::BACKWARD);
    }

    return x == _head_x && y == _head_y;
}

int32_t ToolHead::_internal_to_real(int32_t i, int32_t constant)
{
    return i * _internal_resolution / constant;
}

int32_t ToolHead::_mm_to_internal(double mm)
{
    return mm * 1000 / _internal_resolution;
}

int32_t ToolHead::_real_head_x()
{
    return _motor_unit->position_x() / _motor_unit->resolution_x();
}

int32_t ToolHead::_real_head_y()
{
    return _motor_unit->position_y() / _motor_unit->resolution_y();
}
