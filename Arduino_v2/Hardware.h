/*
Constants for hardware pins, constant values, and defaults
*/
#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <Arduino.h>

// Stepper pins
#define TOP_STP_PIN 3     // Pin for stepping the top motor
#define TOP_DIR_PIN 6     // Pin for changing the top motor direction
#define BOT_STP_PIN 2     // Pin for stepping the bottom motor
#define BOT_DIR_PIN 5     // Pin for changing the bottom motor direction
#define ENABLE_PIN 8      // Pin to enable both motors
#define HALF_STEP_PIN A0  // Pin to enable half stepping
#define QRTR_STEP_PIN 4   // Pin to enable quarter stepping
#define SCREEN_RST 52
#define SCREEN_CE 53
#define SCREEN_DC 51
#define SCREEN_DIN 49
#define SCREEN_CLK 47

// Pen servo pin
#define SERVO_PIN 10  // Pin for the servo motor holding the pen

// Spin directions
#define CW LOW    // Direction value to turn clockwise
#define CCW HIGH  // Direction value to turn counter-clockwise
// Stepper enable values
#define ENABLE_STEPPERS LOW
#define DISABLE_STEPPERS HIGH

// LCD screen settings
#define SCREEN_UPDATE_PERIOD 300  // Time in milliseconds between screen updates
#define SCREEN_CONTRAST 0xBF
#define SCREEN_BIAS 0x13
#define SCRN_CH_WD 7  // Width of a character in pixels
#define SCRN_WD 84    // Width of the screen in pixels

// Defaults
#define DEFAULT_UP 50               // Angle for when the pen is up off the paper
#define DEFAULT_DOWN 70             // Angle when pen is down on the paper
#define DEFAULT_STEPPER_DELAY 2000  // Microsecond delay between stepper pulses
#define DEFAULT_DRAWING_SPEED 250   // Default speed for the stepper motor while drawing
#define DEFAULT_PEN_DELAY 300       // Millisecond delay to pause while the pen goes up/down
#define SPEED_MULTIPLIER 1          // Multiplier to convert from mm/s to whole-steps/s
#define MIN_STEPS_PER_SEC 500       // Minimum steps per second for the stepper motor
#define MAX_MICROSTEP 8             // Maximum microstepping setting

// Serial communication signals
#define DRAW_DONE 0xAB
#define DRAW_START_SENDING 0xFA
#define DRAW_BUFFER_EMPTY 0xFF
#define HANDSHAKE 0x55

void penDraw();
void penMove();
void updateStepperSpeed(uint16_t speed);

#endif  // HARDWARE_H_