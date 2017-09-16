#include "motor_unit.h"
#include "Arduino.h"
#include "utility.h"

const uint8_t Motor::PHASES[8] = { 0x1, 0x5, 0x4, 0x6, 0x2, 0xA, 0x8, 0x9 };

Motor::Motor()
{
}

Motor::Motor(int32_t resolution, bool inverted = false)
{
    _current_phase = 0;
    _stopped = true;
    position = 0;
    this->resolution = resolution;
    _inverted = inverted;
}

void Motor::step(Direction dir)
{
    int next_phase = dir == FORWARD ? 1 : -1;
    position += next_phase > 0 ? resolution : -resolution;

    if (_inverted)
        next_phase = -next_phase;
    _stopped = false;
    _current_phase = (_current_phase + (next_phase + 8)) % 8;
}

void Motor::stop()
{
    _stopped = true;
}

bool Motor::stopped()
{
    return _stopped;
}

uint8_t Motor::phase()
{
    return _stopped ? 0 : Motor::PHASES[_current_phase];
}

MotorUnit::MotorUnit()
{
}

MotorUnit::MotorUnit(PCF* pcf)
{
    _motor_x = Motor(100, true);
    _motor_y = Motor(121);
    _pcf = pcf;
}

void MotorUnit::go_to(int32_t x, int32_t y)
{
    while (true)
    {
        bool reached_x = abs(_motor_x.position - x) < _motor_x.resolution;
        bool reached_y = abs(_motor_y.position - y) < _motor_y.resolution;
        if (reached_x && reached_y)
            break;
        if (!reached_x)
        {
            Motor::Direction direction = ((x - _motor_x.position) > 0) ? Motor::FORWARD : Motor::BACKWARD;
            _motor_x.step(direction);
        }
        if (!reached_y)
        {
            Motor::Direction direction = ((y - _motor_y.position) > 0) ? Motor::FORWARD : Motor::BACKWARD;
            _motor_y.step(direction);
        }
        _update_motor_phases();
        delay(2);
    }
}

void MotorUnit::step_x(Motor::Direction direction)
{
    _motor_x.step(direction);
    _update_motor_phases();
}

void MotorUnit::step_y(Motor::Direction direction)
{
    _motor_y.step(direction);
    _update_motor_phases();
}

void MotorUnit::stop()
{
    _motor_x.stop();
    _motor_y.stop();
    _update_motor_phases();
}

bool MotorUnit::stopped()
{
    return _motor_x.stopped() && _motor_y.stopped();
}

int32_t MotorUnit::position_x()
{
    return _motor_x.position;
}

int32_t MotorUnit::position_y()
{
    return _motor_y.position;
}

int32_t MotorUnit::resolution_x()
{
    return _motor_x.resolution;
}

int32_t MotorUnit::resolution_y()
{
    return _motor_y.resolution;
}

void MotorUnit::_update_motor_phases()
{
    uint8_t phase_x = _motor_x.phase();
    uint8_t phase_y = _motor_y.phase();
    _pcf->write(phase_y << 4 | phase_x);
}
