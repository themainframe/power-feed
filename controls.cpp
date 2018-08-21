#include "Arduino.h"
#include "defs.h"
#include "debug.h"

// Keep track of the number of rotary turns since last check.
volatile int rotary_turns = 0;

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
            rotary_turns += digitalRead(PIN_ROTARY_A) == HIGH ? 1 : -1;
        }
        last_rotary_detent_time_ms = now;
    }
}

/**
 * Initialise control interrupts.
 */
void attach_control_interrupts()
{
    INFO("attaching ISRs for controls")
    PCICR |= 0b00000100;
    PCMSK2 |= 0b10000000;
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