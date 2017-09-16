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

    int32_t position;
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
    MotorUnit(PCF* pcf);
    void go_to(int32_t x, int32_t y);
    void step_x(Motor::Direction direction);
    void step_y(Motor::Direction direction);

    void stop();
    bool stopped();

    int32_t position_x();
    int32_t position_y();
    int32_t resolution_x();
    int32_t resolution_y();

private:
    void _update_motor_phases();

    Motor _motor_x;
    Motor _motor_y;
    PCF* _pcf;
};

#endif