#include <Wire.h>
#include <stdint.h>
#include <Keypad.h>

#include "pcf.h"
#include "motor_unit.h"
#include "tool_head.h"
#include "line.h"
#include "gcode_io.h"
#include "gcode_processor.h"
#include "configuration.h"
#include "utility.h"

#define COMMUNICATION_TIMEOUT 10000

PCF motor_pcf(32);
MotorUnit unit(&motor_pcf);
Tool tool(6, 122, 180);
ToolHead head(&unit, &tool);
GCodeIO gio;
GCodeProcessor gproc;
Configuration config;
bool running_job = false;

const byte ROWS = 3; // Three rows
const byte COLS = 3; // Three columns
                     // Define the Keymap
char keys[ROWS][COLS] = {
    { '1','2','3' },
    { '4','5','6' },
    { '7','8','9' }
};
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = { 3, 4, 5 };
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = { 7, 8, 9 };

// Create the Keypad
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Movement move;
DrawingLine line;
HeightChange height_change;

enum Opcode {NOTHING, LINE, MOVE, HEIGHT};
Opcode op_code = NOTHING;
uint32_t last_op_timestamp = 0;
bool a = false;

void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    gio.setup();
    tool.setup();
}

void loop() {
    /*static int count = 2;
    if (count)
    {
        DrawingLine l = head.create_line_to(50, 50);
        while (!head.process(&l));
        l = head.create_line_to(0, 0);
        while (!head.process(&l));
        count--;
    }
    return;*/

    // Buffer incoming data to internal buffer because 
    // UART buffers are very small
    //gio.buffer_incoming();
    if (!head.is_idle() && time_diff(millis(), last_op_timestamp) > 2000)
        head.idle();

    if (running_job)
    {
        if (op_code != NOTHING)
        { // Command in progress
            switch (op_code)
            {
            case MOVE:
                if (head.process(&move))
                    op_code = NOTHING;
                break;
            case LINE:
                if (head.process(&line))
                    op_code = NOTHING;
                break;
            case HEIGHT:
                if (head.process(&height_change))
                    op_code = NOTHING;
                break;
            default:
                break;
            }
            last_op_timestamp = millis();
        }
        else if (time_diff(millis(), last_op_timestamp) > 15)
        { // Get new command
            String str = gio.receive(COMMUNICATION_TIMEOUT);
            if (str == "")
            {
                running_job = false;
                return;
            }
            gproc.process(str);

            int16_t idx;
            if ((idx = gproc.token_idx('G')) != -1)
            {
                switch (gproc.token_int(idx))
                {
                case 0: {
                    int16_t x_idx = gproc.token_idx('X');
                    int16_t y_idx = gproc.token_idx('Y');
                    int16_t z_idx = gproc.token_idx('Z');
                    if (x_idx != -1 && y_idx != -1)
                    {
                        double x = gproc.token_double(x_idx);
                        double y = gproc.token_double(y_idx);
                        move = head.create_movement_to(x, y);
                        op_code = MOVE;
                    }
                    else if (z_idx != -1)
                    {
                        double z = gproc.token_double(z_idx);
                        height_change = head.create_height_change_to(z);
                        op_code = HEIGHT;
                    }
                    
                    break;
                }
                case 1: {
                    int16_t x_idx = gproc.token_idx('X');
                    int16_t y_idx = gproc.token_idx('Y');
                    int16_t z_idx = gproc.token_idx('Z');
                    if (x_idx != -1 && y_idx != -1)
                    {
                        double x = gproc.token_double(x_idx);
                        double y = gproc.token_double(y_idx);
                        line = head.create_line_to(x, y);
                        op_code = LINE;
                    }
                    else if (z_idx != -1)
                    {
                        double z = gproc.token_double(z_idx);
                        height_change = head.create_height_change_to(z);
                        op_code = HEIGHT;
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
    else
    {
        if (config.finished())
        {
            running_job = gio.check_new_request();
            if (running_job)
            {
                // Start new job
                op_code = NOTHING;
                return;
            }
        }
        else
        {
            bool activity = kpd.getKeys();
            config.process(kpd, activity, unit, tool);
        }
    }
}
