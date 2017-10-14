#ifndef MOTOR_UNIT_H
#define MOTOR_UNIT_H

#include "pcf.h"

class Motor
{
public:
    enum Direction { FORWARD, BACKWARD };
    static const uint8_t PHASES[];

    Motor();
    Motor(int32_t resolution, bool inverted = false);

    void step(Direction dir);
    void stop();
    bool stopped();
    uint8_t phase();

    int32_t resolution;
private:
    uint8_t _current_phase;
    bool _inverted;
    bool _stopped;
};

class MotorUnit
{
public:
    MotorUnit();
    MotorUnit(PCF* pcf, int limit_x_pin, int limit_y_pin);
    void go_to(int32_t x, int32_t y);
    void step_x(Motor::Direction direction);
    void step_y(Motor::Direction direction);

    void stop();
    bool stopped();

    int32_t resolution_x();
    int32_t resolution_y();

private:
    enum MotorId {MOTOR_X=0, MOTOR_Y, MOTOR_ID_CNT};
    enum MovementLimit {NEG_LIMIT, WITHIN_LIMITS, POS_LIMIT};

    void _update_limit(MotorId motor_id);
    bool _check_limit(MotorId motor_id, Motor::Direction direction);
    void _step(MotorId motor_id, Motor::Direction direction);
    void _update_motor_phases();

    Motor _motor[MOTOR_ID_CNT];
    Motor::Direction _last_motor_direction[MOTOR_ID_CNT];
    MovementLimit _motor_limit[MOTOR_ID_CNT];
    int _limit_pins[MOTOR_ID_CNT];

    PCF* _pcf;
};

#endif