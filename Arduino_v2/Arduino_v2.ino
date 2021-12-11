/*
Main file for the Arduino code
Contains:
- startup and loop functions
- shared buffer for the drawing and manual control modes
- list of functions for the runtime modes
*/

#include <TimerOne.h>
#include "Hardware.h"
#include "RuntimeModes.h"

void (*runtime_mode)(void); // The current runtime mode

Servo penServo;
uint8_t penUpAngle, penDownAngle;

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
    servo.attach(SERVO_PIN);
    servo.write(penUpAngle);
}

void setup()
{
    // Set up serial
    Serial.begin(9600);
    Serial.println("setup()");
    // Initialize the timer (not start)
    Timer1.initialize(DEFAULT_DELAY); // 6 seconds
    runtime_mode = acceptingCommands;
    hardwareInit();
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