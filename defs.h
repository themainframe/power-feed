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
#define UM_PER_ROTATION 2000

// The width of a single step pulse in microseconds
#define STEP_PULSE_WIDTH_US 4

// The total number of steps for one rotation of the motor
#define STEPS_PER_ROTATION 12800

// The number of steps to move the table by 1um
#define STEPS_PER_UM (STEPS_PER_ROTATION / UM_PER_ROTATION)

// The rate of the "rapid move" in micrometers/second
#define FEEDRATE_RAPID_UM_SEC 7000

#endif