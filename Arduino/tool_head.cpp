#include "tool_head.h"
#include "Arduino.h"
#include "utility.h"

#define MIN_MOTOR_DELAY_US 3000//1300
#define MAX_MOTOR_DELAY_US 10000
#define MOTOR_DELAY_RANGE_US (MAX_MOTOR_DELAY_US - MIN_MOTOR_DELAY_US)
#define TOOL_DELAY_MS 1000

ToolHead::ToolHead(MotorUnit& motor_unit, Tool& tool) :
    _motor_unit(motor_unit),
    _tool(tool)
{
    _internal_resolution = min(_motor_unit.resolution_x(), _motor_unit.resolution_y());
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
    if (line->finished())
        return true;

    uint32_t delay = MAX_MOTOR_DELAY_US - (line->speed() * MOTOR_DELAY_RANGE_US) / 100;
    if (time_diff(timestamp, line->timestamp()) < delay)
        return false;

    line->timestamp(timestamp);

    line->step();
    if (line->finished())
        return false;

    _linear_move_toward(line->x(), line->y());

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

    _fast_move_toward(move->x(), move->y());

    return false;
}

bool ToolHead::process(HeightChange* change)
{
    if (!change->processed())
    {
        if (change->z() < 0)
            _tool.change_state(Tool::DOWN);
        else
            _tool.change_state(Tool::UP);
        change->processed(true);
        change->timestamp(millis());
    }
    return time_diff(millis(), change->timestamp()) >= TOOL_DELAY_MS;
}

void ToolHead::idle()
{
    _motor_unit.stop();
}

bool ToolHead::is_idle()
{
    return _motor_unit.stopped();
}

void ToolHead::_basic_move_toward(int32_t x, int32_t y)
{
    if (x > _real_head_x())
        _motor_unit.step_x(Motor::FORWARD);
    else if (x < _real_head_x())
        _motor_unit.step_x(Motor::BACKWARD);

    if (y > _real_head_y())
        _motor_unit.step_y(Motor::FORWARD);
    else if (y < _real_head_y())
        _motor_unit.step_y(Motor::BACKWARD);
}

int32_t difference_sign(int32_t a, int32_t b)
{
    return a > b ? 1 : a < b ? -1 : 0;
}

bool ToolHead::_linear_move_toward(int32_t x, int32_t y) {
    int32_t dx = difference_sign(x, _head_x);
    int32_t dy = difference_sign(y, _head_y);
    if (dx == 0 && dy == 0)
        return true;

    int32_t target_x = _head_x + dx;
    int32_t target_y = _head_y + dy;

    int32_t target_real_x = _internal_to_real(target_x, _motor_unit.resolution_x());
    int32_t target_real_y = _internal_to_real(target_y, _motor_unit.resolution_y());

    _basic_move_toward(target_real_x, target_real_y);

    _head_x = target_x;
    _head_y = target_y;

    return false;
}

bool ToolHead::_fast_move_toward(int32_t x, int32_t y) {
    int32_t dx = difference_sign(x, _head_x);
    int32_t dy = difference_sign(y, _head_y);
    if (dx == 0 && dy == 0)
        return true;

    int32_t real_x = _real_head_x();
    int32_t real_y = _real_head_x();

    int32_t target_x = _head_x;
    int32_t target_y = _head_y;
    int32_t target_real_x = real_x;
    int32_t target_real_y = real_y;

    while (dx != 0 && target_real_x == real_x)
    {
        target_x += dx;
        target_real_x = _internal_to_real(target_x, _motor_unit.resolution_x());
    }

    while (dy != 0 && target_real_y == real_y)
    {
        target_y += dy;
        target_real_y = _internal_to_real(target_y, _motor_unit.resolution_y());
    }
    
    _basic_move_toward(target_real_x, target_real_y);

    _head_x = target_x;
    _head_y = target_y;

    return false;
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
    return _internal_to_real(_head_x, _motor_unit.resolution_x());
}

int32_t ToolHead::_real_head_y()
{
    return _internal_to_real(_head_y, _motor_unit.resolution_y());
}
