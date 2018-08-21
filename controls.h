#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include "Arduino.h"

/**
 * A structure to store the state of all the buttons at once.
 */
typedef struct
{
    unsigned int fast_left_press_count;
    unsigned int slow_left_press_count;
    unsigned int slow_right_press_count;
    unsigned int fast_right_press_count;
    unsigned int stop_press_count;
    unsigned int rotary_press_count;

} button_states_t;

void attach_control_interrupts();
int get_rotary_turns();
void get_button_states(button_states_t* states);

#endif