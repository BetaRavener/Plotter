#ifndef TOOL_H
#define TOOL_H

#include <Servo.h>

class Tool
{
public:
    enum State {UP, DOWN};

    Tool(int pin, int min_height=0, int max_height=180);
    void setup();

    State state();
    void change_state(State state);
    int height(State state);
    void change_height(State state, int val);
private:
    void _update_output();
    
    State _state;
    int _servo_pin;
    int _up_height;
    int _down_height;
    int _min_height;
    int _max_height;
    Servo _servo;
};

#endif // ! TOOL_H
