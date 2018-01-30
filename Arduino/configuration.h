#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Keypad.h>
#include "motor_unit.h"
#include "tool.h"

class Configuration
{
public:
	Configuration();
    void process(Keypad& keypad, bool activity, MotorUnit& unit, Tool& tool);
    bool finished();
private:
    enum Stages {START = 0, XY, FINISHED};
    int _stage;
};

#endif