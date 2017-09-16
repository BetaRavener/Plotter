#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Keypad.h>
#include "motor_unit.h"
#include "tool.h"

class Configuration
{
public:
    void process(Keypad& keypad, bool activity, MotorUnit& unit, Tool& tool);
    bool finished();
private:
    enum Stages {Z_UP = 0, XY, Z_DOWN, FINISHED};
    int _stage;
};

#endif