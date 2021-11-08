/*
Main file for the Arduino code
Contains:
- startup and loop functions
- shared buffer for the drawing and manual control modes
- list of functions for the runtime modes
*/

#include <TimerOne.h>
#include "RuntimeModes.h"

void (*runtime_mode)(void);  // The current runtime mode

void setup()
{
    Serial.begin(9600);
    Serial.println("setup()");
    Timer1.initialize(6000000);  // 6 seconds
    runtime_mode = accepting_commands;
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