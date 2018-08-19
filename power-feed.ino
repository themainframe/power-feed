#include "defs.h"
#include "timer.h"
#include "debug.h"

/*
power-feed - An Arduino-based single-axis milling machine power feed controller.
Ported from Chris Mower's original millcode project (https://github.com/kintekobo/millcode).
*/

/**
 * Define the possible movement directions.
 */
typedef enum {
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} direction_t;

/**
 * Define the possible movement modes.
 */
typedef enum {
    MODE_STOP,
    MODE_PRECISION,
    MODE_RAPID
} mode_t;

// The precision feedrate (um/sec)
// The "rapid" feedrate is fixed and defined in defs.h
volatile unsigned long desired_feedrate_precision_um_sec = 1000;

// The current direction (default LEFT)
volatile direction_t desired_direction = DIRECTION_LEFT;

// The current mode (default STOP)
volatile mode_t desired_mode = MODE_STOP;

// The reason why the timer has been automatically stopped
volatile extern timer_stop_reason_t timer_stop_reason;

// Has the hardware been configured according to the desired_* parameters above?
volatile bool hardware_configured = false;

/**
 * Set up the system.
 */
void setup()
{
    // Intialise serial debugging
    Serial.begin(115200);
    INFO("Power feed system startup")

    // Initialise timers
    timer_init();

    // Initialise ports & pins
    pinMode(PIN_ENDSTOP, INPUT_PULLUP);
}

/**
 * Our main loop inspecs the deisred state (desired_ variables) and configures the timer/output pins if required.
 * All operational halt (stop button, endstop) actions are detected in the timer interrupt itself for safety.
 */
void loop()
{   
    // Check to see if the timer was automatically stopped due to the endstop being reached
    if (timer_stop_reason == REASON_ENDSTOP) {
        
        // Reset the timer stop reason
        INFO("timer was stopped automatically due to reaching endstop.")
        timer_stop_reason = REASON_NONE;

        // Change our mode to STOP
        desired_mode = MODE_STOP;

        // TODO: Set the enable state
        
        return;
    }

    // Does the hardware need to be configured?
    if (!hardware_configured) {

        switch (desired_mode) {
            
            case MODE_STOP:

                // TODO: Set the direction and enable state

                // Stop
                timer_stop();

                break;

            case MODE_RAPID:

                // TODO: Set the direction and enable state

                // Move at the rapid feedrate
                reconfigure_timer_for_feedrate(FEEDRATE_RAPID_UM_SEC);

                break;

            case MODE_PRECISION:

                // TODO: Set the direction and enable state
                
                // Move at the precision feedrate
                reconfigure_timer_for_feedrate(desired_feedrate_precision_um_sec);

                break;
        }

        // We're set up
        hardware_configured = true;
    }

    handle_commands();
}

/**
 * Service any commands issued on the serial port.
 */
void handle_commands()
{
    // Service commands on the serial port
    if (Serial.available() > 0) {
        
        String commandString = Serial.readStringUntil('\r'); 

        // Check for valid commands
        if (commandString.startsWith("stop")) {

            // Stop.
            desired_mode = MODE_STOP;
            hardware_configured = false;

        } else if (commandString.startsWith("fl")) {

            // Change to Rapid Left movement
            desired_mode = MODE_RAPID;
            desired_direction = DIRECTION_LEFT;
            hardware_configured = false;

        } else if (commandString.startsWith("fr")) {

            // Change to Rapid Right movement
            desired_mode = MODE_RAPID;
            desired_direction = DIRECTION_RIGHT;
            hardware_configured = false;

        } else if (commandString.startsWith("sl")) {

            // Change to Slow Left movement
            desired_mode = MODE_PRECISION;
            desired_direction = DIRECTION_LEFT;
            hardware_configured = false;

        } else if (commandString.startsWith("sr")) {

            // Change to Slow Right movement
            desired_mode = MODE_PRECISION;
            desired_direction = DIRECTION_RIGHT;
            hardware_configured = false;

        } else if (commandString.startsWith("?")) {

            INFO("Possible commands: fl, sl, stop, sr, fr, 1000 (Eg. for 1000um/sec precision feedrate)")
            return;
        
        } else {

            // Change the "precision" feedrate
            desired_feedrate_precision_um_sec = commandString.toInt();
            hardware_configured = false;
            
        }
    }
}