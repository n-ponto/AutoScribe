/*
Main file for the Arduino code
Contains:
- startup and loop functions
- shared buffer for the drawing and manual control modes
- list of functions for the runtime modes
*/

#include <TimerOne.h>
#include <Servo.h>
#include "Hardware.h"
#include "RuntimeModes.h"
#include "Display.h"

void (*runtime_mode)(void); // The current runtime mode

Servo penServo;
uint8_t penUpAngle, penDownAngle;
uint16_t stepperDelay; // Microsecond delay between stepper pulses

// Initialize the output pins for the stepper CNC sheild
void hardwareInit()
{
    // Initilize the stepper pins
    pinMode(TOP_STP_PIN, OUTPUT);
    pinMode(TOP_DIR_PIN, OUTPUT);
    pinMode(BOT_STP_PIN, OUTPUT);
    pinMode(BOT_DIR_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, DISABLE_STEPPERS);
    // initialize the pen servo pin
    penUpAngle = DEFAULT_UP;
    penDownAngle = DEFAULT_UP;
    penServo.attach(SERVO_PIN);
    penServo.write(penUpAngle);
}

void setup()
{
    Serial.begin(9600);
    stepperDelay = DEFAULT_STEPPER_DELAY;
    Timer1.initialize(stepperDelay);
    runtime_mode = acceptingCommands;
    hardwareInit();
    displayInit();
}

// Always reading and buffering instructions from serial
void loop()
{
    // Call the function for the current runtime mode
    runtime_mode();
}

void setRuntimeMode(void (*fn)(void))
{
    runtime_mode = fn;
}