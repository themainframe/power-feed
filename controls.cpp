#include "Arduino.h"
#include "controls.h"
#include "defs.h"
#include "debug.h"

// Keep track of the number of rotary turns since last check.
static volatile int rotary_turns = 0;

// Keep track of button pushes
static volatile button_states_t button_states = {
    .fast_left_press_count = 0,
    .slow_left_press_count = 0,
    .slow_right_press_count = 0,
    .fast_right_press_count = 0,
    .stop_press_count = 0,
    .rotary_press_count = 0
};

/**
 * ISR for buttons being pressed.
 */
ISR(PCINT0_vect)
{
    // Primitive debouncing
    delayMicroseconds(1000);

    // Capture the state of all buttons
    button_states.fast_left_press_count |= digitalRead(PIN_FAST_LEFT) == LOW;
    button_states.slow_left_press_count |= digitalRead(PIN_SLOW_LEFT) == LOW;
    button_states.slow_right_press_count |= digitalRead(PIN_SLOW_RIGHT) == LOW;
    button_states.fast_right_press_count |= digitalRead(PIN_FAST_RIGHT) == LOW;
    button_states.stop_press_count |= digitalRead(PIN_STOP) == LOW;
    button_states.rotary_press_count |= digitalRead(PIN_ROTARY_SW) == LOW;
}

/**
 * ISR for the rotary encoder being turned.
 */
ISR(PCINT2_vect)
{
    // Keep track of the last detent time to provide a little debouncing (mainly to limit the update rate)
    static unsigned long last_rotary_detent_time_ms = 0;

    if (digitalRead(PIN_ROTARY_B) == LOW) {
        unsigned long now = millis();
        if (now - last_rotary_detent_time_ms > 5) {
            // Increment or decrement rotary_turns based on which direction the rotary was turned
            rotary_turns += (digitalRead(PIN_ROTARY_A) == HIGH ? 1 : -1) * ENCODER_DIRECTION;
        }
        last_rotary_detent_time_ms = now;
    }
}

/**
 * Initialise control interrupts.
 */
void attach_control_interrupts()
{
    DEBUG("attaching ISRs for controls")

    // Pin Change Interrupts enabled for PCIE0 and PCIE2
    PCICR |= bit(PCIE0) | bit(PCIE2);

    // 5 buttons + rotary on PCIE0
    PCMSK0 |= 0b00111111;

    // Just the single rotary quadrature on PCIE2
    PCMSK2 |= 0b10000000;
}

/**
 * This subroutine safely copies the state of the buttons into the destination memory
 * then resets the internal button states.
 */
void get_button_states(button_states_t* states)
{
    noInterrupts();
    
    // Copy the states first
    states->fast_left_press_count = button_states.fast_left_press_count;
    states->slow_left_press_count = button_states.slow_left_press_count;
    states->slow_right_press_count = button_states.slow_right_press_count;
    states->fast_right_press_count = button_states.fast_right_press_count;
    states->stop_press_count = button_states.stop_press_count;
    states->rotary_press_count = button_states.rotary_press_count;
    
    // Now reset them
    button_states.fast_left_press_count = 0;
    button_states.slow_left_press_count = 0;
    button_states.slow_right_press_count = 0;
    button_states.fast_right_press_count = 0;
    button_states.stop_press_count = 0;
    button_states.rotary_press_count = 0;
    interrupts();
}

/**
 * Get & reset the number of rotary turns since the last check.
 * 
 * We could be preempted during this function, so interrupts are disabled for the critical section
 * of reading and writing (resetting) the number of turns.
 */
int get_rotary_turns()
{
    int rotary_turns_atomic;
    
    noInterrupts();
    rotary_turns_atomic = rotary_turns;
    rotary_turns = 0;
    interrupts();

    return rotary_turns_atomic;
}