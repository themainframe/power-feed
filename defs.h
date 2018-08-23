/*
Definitions for Arduino peripheral pins, milling table geometry and stepping rate parameters.

The Arduino pins and table geometry measurements are consistent with the pins used in
the original project by Chris Mower (https://github.com/kintekobo/millcode).
*/

#ifndef __DEFS_H__
#define __DEFS_H__

// Output pins
#define PIN_STEP            3
#define PIN_DIRECTION       4
#define PIN_ENABLE          5

// Input pins
#define PIN_FAST_LEFT       13
#define PIN_SLOW_LEFT       12
#define PIN_SLOW_RIGHT      11
#define PIN_FAST_RIGHT      10
#define PIN_STOP            9
#define PIN_ROTARY_SW       8
#define PIN_ROTARY_A        7
#define PIN_ROTARY_B        6
#define PIN_ENDSTOP         2

// The number of um travelled with one leadscrew rotation
#define UM_PER_ROTATION 2000L

// The width of a single step pulse in microseconds
#define STEP_PULSE_WIDTH_US 4L

// The total number of steps for one rotation of the motor
#define STEPS_PER_ROTATION 12800L

// The number of steps to move the table by 1um
#define STEPS_PER_MM (STEPS_PER_ROTATION / (UM_PER_ROTATION / 1000))

// The default rate of the "precision move" in micrometers/second
#define FEEDRATE_PRECISION_DEFAULT_UM_SEC 1000

// The rate of the "rapid move" in micrometers/second
#define FEEDRATE_RAPID_UM_SEC 7000

// The rate change adjusted by each detent of the rotary encoder
#define FEEDRATE_ADJUST_STEP_UM_SEC 50

// The direction multiplier for the rotary encoder
// Change to -1 if you can't swap the quadrature lines around easily to invert the direction
#define ENCODER_DIRECTION 1

// Allow backing away from the endstop in the other direction
// May sometimes be unsafe. Moving away from endstops manually is the alternative.
#define ALLOW_BACK_AWAY_FROM_ENDSTOP 1

// Custom character indexes for use on the LCD
#define LCD_CHAR_ARROW_RIGHT 0
#define LCD_CHAR_ARROW_LEFT 1
#define LCD_CHAR_STOP 2

#endif