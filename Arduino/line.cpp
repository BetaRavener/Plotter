#include "line.h"
#include "Arduino.h"
#include "utility.h"

#define SLOW_ZONE 200

Line::Line(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    _x1 = x1;
    _y1 = y1;
    _x2 = x2;
    _y2 = y2;
}

DrawingLine::DrawingLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
    Line(x1, y1, x2, y2)
{
    int32_t dx, dy;
    _left_right = x2 >= x1;

    //Bressenhamov algoritmus
    dx = _left_right ? (x2 - x1) : (x1 - x2);
    dy = abs(y2 - y1);
    _make_swap = 0;
    if (dx < dy)
    {
        swap(&x1, &y1);
        swap(&x2, &y2);

        _left_right = x2 >= x1;

        dx = _left_right ? (x2 - x1) : (x1 - x2);
        dy = abs(y2 - y1);
        _make_swap = 1;
    }

    // Fill context with necessary data
    _x1 = x1; _y1 = y1;
    _x2 = x2; _y2 = y2;
    _dx = dx; _dy = dy;
    _P1 = 2 * dy;
    _P2 = _P1 - 2 * dx;
    _P = 2 * dy - dx;
    _x = x1; _y = y1;
    _ystep = y2 >= y1 ? 1 : -1;
    _finished = false;
    _speed = 0;
}

void DrawingLine::step()
{
    if (_finished)
        return;

    // Is the head in final position?
    if (_x == _x2)
    {
        _finished = true;
        return;
    }

    // Perform next step of algorithm
    if (_P >= 0)
    {
        _P += _P2;
        _y += _ystep;
    }
    else
    {
        _P += _P1;
    }

    if (_left_right)
    {
        _x++;
    }
    else
    {
        _x--;
    }

    // Update speed:
    // Accelerate at the beggining
    // Slow down at the end
    int b_dist = abs(_x - _x1) + abs(_y - _y1);
    int e_dist = abs(_x2 - _x) + abs(_y2 - _y);
    int min_dist = min(b_dist, e_dist);
    if (min_dist < SLOW_ZONE)
        _speed = (100 * min_dist) / SLOW_ZONE;
    else
        _speed = 100;
}

bool DrawingLine::finished()
{
    return _finished;
}

int32_t DrawingLine::x()
{
    return _make_swap ? _y : _x;
}

int32_t DrawingLine::y()
{
    return _make_swap ? _x : _y;
}

uint8_t DrawingLine::speed()
{
    return _speed;
}
