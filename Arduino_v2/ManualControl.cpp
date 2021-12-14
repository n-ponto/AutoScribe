/*
Contains all code specific to the Manual Control runtime mode.
*/
#include <TimerOne.h>
#include <Servo.h>
#include "RuntimeModes.h"
#include "Hardware.h"

// Possible bytes passed from serial
#define STOP 0
#define SP_P 1
#define SP_R 2
#define UP_P 3
#define UP_R 4
#define DN_P 5
#define DN_R 6
#define LT_P 7
#define LT_R 8
#define RT_P 9
#define RT_R 10

extern Servo penServo;
extern uint8_t penUpAngle, penDownAngle;

bool movePen, penUp;
bool stepx, stepy;
char writeVal; // Only set by interrupt

void manualControlInterrupt()
{
    // Move steppers
    if (stepx)
        digitalWrite(TOP_STP_PIN, writeVal);
    if (stepy)
        digitalWrite(BOT_STP_PIN, writeVal);
    writeVal = !writeVal;
}

void startManualControl()
{
    movePen = false;
    penUp = true;
    stepx = stepy = false;
    writeVal = HIGH;
    Serial.println("Starting manual control mode.");
    digitalWrite(ENABLE_PIN, ENABLE_STEPPERS); // Enable steppers
    Timer1.attachInterrupt(manualControlInterrupt);
    Timer1.start();
}

void endManualControl()
{
    Timer1.stop();                              // Stop the timer
    Timer1.detachInterrupt();                   // Remove the interrupt
    digitalWrite(ENABLE_PIN, DISABLE_STEPPERS); // Disable steppers
    setRuntimeMode(acceptingCommands);          // Return to Accepting Commands
    Serial.println("Manual control mode ended.");
}

void manualControl()
{
    /*
    Read a byte from serial corresponding to a direction (or moving pen up/down)
    Convert the direction into a specific command (how to move which motors)
    If the queue is full then stop reading

    When receives the stop signal, immediately stop the timer interrupt
    Then change the mode back to accepting commands
    */
    char key;
    startManualControl();
    while (1)
    {
        // Wait for serial
        while (Serial.available() < 1)
        {
        }

        Serial.readBytes((char*)&key, 1);
        if (key == STOP)
            break;

        switch (key)
        {
        case (SP_P):
            penServo.write(penDownAngle);
            break;
        case (SP_R):
            penServo.write(penUpAngle);
            break;
        case (UP_P):
            digitalWrite(BOT_DIR_PIN, CW);
            stepy = true;
            break;
        case (UP_R):
            stepy = false;
            break;
        case (DN_P):
            digitalWrite(BOT_DIR_PIN, CCW);
            stepy = true;
            break;
        case (DN_R):
            stepy = false;
            break;
        case (LT_P):
            digitalWrite(TOP_DIR_PIN, CCW);
            stepx = true;
            break;
        case (LT_R):
            stepx = false;
            break;
        case (RT_P):
            digitalWrite(TOP_DIR_PIN, CW);
            stepx = true;
            break;
        case (RT_R):
            stepx = false;
            break;
        }
    }
    endManualControl();
}