#include <LiquidCrystal_I2C.h>
#include "defs.h"
#include "debug.h"
#include "timer.h"
#include "controls.h"

/*
power-feed - An Arduino-based single-axis milling machine power feed controller.
Ported from Chris Mower's original millcode project (https://github.com/kintekobo/millcode).
*/

// Initialise LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Define the appearance of the custom characters
uint8_t char_arrow_right[8] = { 0x10, 0x18, 0x1C, 0x1E, 0x1E, 0x1C, 0x18, 0x10 };
uint8_t char_arrow_left[8] = { 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x01 };
uint8_t char_stop[8] = { 0x00, 0x00, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00 };

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
    pinMode(PIN_FAST_LEFT, INPUT_PULLUP);
    pinMode(PIN_FAST_RIGHT, INPUT_PULLUP);
    pinMode(PIN_SLOW_LEFT, INPUT_PULLUP);
    pinMode(PIN_SLOW_RIGHT, INPUT_PULLUP);
    pinMode(PIN_STOP, INPUT_PULLUP);
    pinMode(PIN_ROTARY_A, INPUT_PULLUP);
    pinMode(PIN_ROTARY_B, INPUT_PULLUP);
    pinMode(PIN_ROTARY_SW, INPUT_PULLUP);

    // Intialise the LCD
    lcd.begin();
	lcd.backlight();

    // Register some custom characters with the LCD for later use
    lcd.createChar(LCD_CHAR_ARROW_LEFT, char_arrow_left);
    lcd.createChar(LCD_CHAR_ARROW_RIGHT, char_arrow_right);
    lcd.createChar(LCD_CHAR_STOP, char_stop);

    // Attach interrupts for controls
    attach_control_interrupts();
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
    }

    // Was the rotary changed?
    int rotary_turns = get_rotary_turns();
    if (rotary_turns != 0) {

        long adjustment = FEEDRATE_ADJUST_STEP_UM_SEC * rotary_turns;

        // Ensure we don't go below 0 feedrate or above the rapid feedrate
        if ((long)desired_feedrate_precision_um_sec + adjustment <= 0) {
            desired_feedrate_precision_um_sec = 0;
        } else if ((long)desired_feedrate_precision_um_sec + adjustment >= FEEDRATE_RAPID_UM_SEC) {
            desired_feedrate_precision_um_sec = FEEDRATE_RAPID_UM_SEC;
        } else {
            desired_feedrate_precision_um_sec = desired_feedrate_precision_um_sec + adjustment;
        }

        hardware_configured = false;
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

        // Update the display to reflect the current state
        DEBUG("updating display contents")
        lcd.clear();

        // Write the top line
        lcd.setCursor(0,0);
        lcd.print("Rate: ");

        // Write symbols describing the current state
        lcd.setCursor(13, 0);
        switch (desired_mode) {
            
            case MODE_STOP:
                lcd.write(' ');
                lcd.write(LCD_CHAR_STOP);
                break;

            case MODE_RAPID:
                lcd.write(desired_direction == DIRECTION_LEFT ? LCD_CHAR_ARROW_LEFT : LCD_CHAR_ARROW_RIGHT);
                lcd.write(desired_direction == DIRECTION_LEFT ? LCD_CHAR_ARROW_LEFT : LCD_CHAR_ARROW_RIGHT);
                break;

            case MODE_PRECISION:
                lcd.write(' ');
                lcd.write(desired_direction == DIRECTION_LEFT ? LCD_CHAR_ARROW_LEFT : LCD_CHAR_ARROW_RIGHT);
                break;
        }

        lcd.setCursor(0, 1);
        String feedrate_text = String((float)((float)desired_feedrate_precision_um_sec / (float)1000), 3) + " mm/s";
        lcd.print(feedrate_text);

        // We're set up
        DEBUG("peripherals updated")
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
            INFO_VAL("Set new desired precision feedrate", desired_feedrate_precision_um_sec)
        }
    }
}