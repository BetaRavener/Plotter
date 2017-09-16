#ifndef LINE_H
#define LINE_H

#include <stdint.h>

#include "timestamped.h"

class Line
{
public:
    Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
protected:
    int32_t _x1;
    int32_t _y1;
    int32_t _x2;
    int32_t _y2;

private:

};

class DrawingLine : public Line, public Timestamped
{
public:
    DrawingLine() : Line(0,0,0,0) {}
    DrawingLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void step();
    bool finished();

    int32_t x();
    int32_t y();
    uint8_t speed();
private:
    int32_t _dx, _dy, _P1, _P2, _P, _x, _y, _ystep;
    uint8_t _speed;
    bool _make_swap, _left_right, _finished;
};

#endif // ! LINE_H
