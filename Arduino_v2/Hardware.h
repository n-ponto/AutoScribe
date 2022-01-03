/*
Constants for hardware pins, constant values, and defaults
*/
#ifndef HARDWARE_H_
#define HARDWARE_H_

// Stepper pins
#define TOP_STP_PIN 3 // Pin for stepping the top motor
#define TOP_DIR_PIN 6 // Pin for changing the top motor direction
#define BOT_STP_PIN 2 // Pin for stepping the bottom motor
#define BOT_DIR_PIN 5 // Pin for changing the bottom motor direction
#define ENABLE_PIN 8  // Pin to enable both motors

// Pen servo pin
#define SERVO_PIN 10 // Pin for the servo motor holding the pen

// Display Pins
#define DSP_PIN_CLK 13 // Clock
#define DSP_PIN_DIN 12 // Data input pin
#define DSP_PIN_DC 11  // Data or command pin
#define DSP_PIN_CE 4  // Chip select pin
#define DSP_PIN_RST 9 // Reset pin
#define DSP_PIN_BL 7  // Backlight pin

// Spin directions
#define CW LOW   // Direction value to turn clockwise
#define CCW HIGH // Direction value to turn counter-clockwise
// Stepper enable values
#define ENABLE_STEPPERS LOW
#define DISABLE_STEPPERS HIGH

// Defaults
#define DEFAULT_UP 50              // Angle for when the pen is up off the paper
#define DEFAULT_DOWN 70            // Angle when pen is down on the paper
#define DEFAULT_STEPPER_DELAY 2000 // Microsecond delay between stepper pulses
#define DEFAULT_PEN_DELAY 50       // Millisecond delay to pause while the pen goes up/down

#endif //HARDWARE_H_