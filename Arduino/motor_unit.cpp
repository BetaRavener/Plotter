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
    this->resolution = resolution;
    _inverted = inverted;
}

void Motor::step(Direction dir)
{
    int next_phase = dir == FORWARD ? 1 : -1;

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

MotorUnit::MotorUnit(PCF* pcf, int limit_x_pin, int limit_y_pin)
{
    _motor[MOTOR_X] = Motor(100, true);
    _motor[MOTOR_Y] = Motor(121);
    _limit_pins[MOTOR_X] = limit_x_pin;
    _limit_pins[MOTOR_Y] = limit_y_pin;
    for (int i = 0; i < MOTOR_ID_CNT; i++)
    {
        pinMode(_limit_pins[i], INPUT);
        _last_motor_direction[i] = Motor::FORWARD;
        _motor_limit[i] = WITHIN_LIMITS;
    }
    _pcf = pcf;
}

//void MotorUnit::go_to(int32_t x, int32_t y)
//{
//    while (true)
//    {
//        bool reached_x = abs(_motor[MOTOR_X].position - x) < _motor[MOTOR_X].resolution;
//        bool reached_y = abs(_motor[MOTOR_Y].position - y) < _motor[MOTOR_Y].resolution;
//        if (reached_x && reached_y)
//            break;
//
//        if (!reached_x)
//        {
//            Motor::Direction direction = ((x - _motor[MOTOR_X].position) > 0) ? Motor::FORWARD : Motor::BACKWARD;
//            _step(MOTOR_X, direction);
//        }
//        if (!reached_y)
//        {
//            Motor::Direction direction = ((y - _motor[MOTOR_Y].position) > 0) ? Motor::FORWARD : Motor::BACKWARD;
//            _step(MOTOR_Y, direction);
//        }
//        _update_motor_phases();
//        delay(2);
//    }
//}

void MotorUnit::step_x(Motor::Direction direction)
{
    _step(MOTOR_X, direction);
    _update_motor_phases();
}

void MotorUnit::step_y(Motor::Direction direction)
{
    _step(MOTOR_Y, direction);
    _update_motor_phases();
}

void MotorUnit::stop()
{
    _motor[MOTOR_X].stop();
    _motor[MOTOR_Y].stop();
    _update_motor_phases();
}

bool MotorUnit::stopped()
{
    return _motor[MOTOR_X].stopped() && _motor[MOTOR_Y].stopped();
}

int32_t MotorUnit::resolution_x()
{
    return _motor[MOTOR_X].resolution;
}

int32_t MotorUnit::resolution_y()
{
    return _motor[MOTOR_Y].resolution;
}

void MotorUnit::_update_limit(MotorId motor_id)
{
    int toggle;
    if (motor_id == MOTOR_X)
        toggle = digitalRead(_limit_pins[MOTOR_X]) == LOW;
    else if (motor_id == MOTOR_Y)
        toggle = digitalRead(_limit_pins[MOTOR_Y]) == LOW;

    if (toggle)
    {
        // Update only when going outside of limits, not when already outside
        if (_motor_limit[motor_id] == WITHIN_LIMITS)
        {
            if (_last_motor_direction[motor_id] == Motor::FORWARD)
                _motor_limit[motor_id] = POS_LIMIT;
            else if (_last_motor_direction[motor_id] == Motor::BACKWARD)
                _motor_limit[motor_id] = NEG_LIMIT;
        }
    }
    else
        _motor_limit[motor_id] = WITHIN_LIMITS;
}

bool MotorUnit::_check_limit(MotorId motor_id, Motor::Direction direction)
{
    if (direction == Motor::FORWARD && _motor_limit[motor_id] == POS_LIMIT)
        return false;
    if (direction == Motor::BACKWARD && _motor_limit[motor_id] == NEG_LIMIT)
        return false;
    return true;
}

void MotorUnit::_step(MotorId motor_id, Motor::Direction direction)
{
    // Update limits based on last movement
    _update_limit(motor_id);

    // Check limits and if motor can't move in the direction, do nothing
    if (!_check_limit(motor_id, direction))
    {
        return;
    }
    _motor[motor_id].step(direction);
    _last_motor_direction[motor_id] = direction;
}

void MotorUnit::_update_motor_phases()
{
    uint8_t phase_x = _motor[MOTOR_X].phase();
    uint8_t phase_y = _motor[MOTOR_Y].phase();
    _pcf->write(phase_y << 4 | phase_x);
}
