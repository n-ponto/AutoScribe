/*
Main file for the Arduino code
Contains:
- startup and loop functions
- shared buffer for the drawing and manual control modes
- list of functions for the runtime modes
*/

#include "RuntimeModes.h"

void setup()
{
    runtime_mode = accepting_commands;
}

// Always reading and buffering instructions from serial
void loop()
{
    // Call the function for the current runtime mode
    runtime_mode();
}