/*
Contains all code specific to the Accepting Commands runtime mode.

- accepting commands function to read commands from serial
- list of commands (functions to run for each commands)
*/

#include <Arduino.h>

#include "Commands.h"

void accepting_commands()
{
    Serial.println("Starting accepting commands runtime mode.");
    // Read a byte (command) from serial
    // Call the function corresponding to that commmand
    // Function will handle reading more data from serial
    // Have functions for switching commands (change the runtime mode for the main loop)
    while (true)
    {
        unsigned char command = 0; // Read byte from serial
        HANDLE(command);
        if (command & RUNTIME_CHANGE)
            break;
    }
    Serial.println("Leaving accepting commands runtime mode.");
}