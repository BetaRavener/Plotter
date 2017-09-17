#ifndef TOOL_HEAD
#define TOOL_HEAD

#include <stdint.h>
#include "motor_unit.h"
#include "tool.h"
#include "line.h"
#include "movement.h"
#include "height_change.h"

class ToolHead
{
public:
    ToolHead(MotorUnit& motor_unit, Tool& tool);

    Movement create_movement_to(double x_mm, double y_mm);
    DrawingLine create_line_to(double x_mm, double y_mm);
    HeightChange create_height_change_to(double z_mm);

    bool process(DrawingLine* line);
    bool process(Movement* move);
    bool process(HeightChange* change);

    void idle();
    bool is_idle();
    
private:
    void _basic_move_toward(int32_t x, int32_t y);
    bool _linear_move_toward(int32_t x, int32_t y);
    bool _fast_move_toward(int32_t x, int32_t y);
    int32_t _internal_to_real(int32_t i, int32_t constant);
    int32_t _mm_to_internal(double mm);
    int32_t _real_head_x();
    int32_t _real_head_y();

    MotorUnit& _motor_unit;
    Tool& _tool;
    int32_t _internal_resolution;
    int32_t _head_x;
    int32_t _head_y;
};

#endif // !TOOL_HEAD
