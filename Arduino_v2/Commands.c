/*
Contains the implementations for all the commands that can be called from the 
Accepting Commands mode
*/

#include "RuntimeModes.h"

void setPenRange()
{
    // Read two bytes, one for the minimum then maximum angle for the pen's
    // movement
    return;
}

void changePenAngle()
{
    // Read one byte and change the pen angle to that value
    return;
}

void moveToCoordinate()
{
    // Read two sets of 2 byte values, one each for the x and y axes
    // Immediately move to that location
    return;
}

void resetHome()
{
    // Reset the current recorded position to the origin (0, 0)
    return;
}

void enterDrawMode()
{
    // Set the runtime mode to drawing to be called from the main loop
    runtime_mode = drawing;
}

void enterManualControlMode()
{
    // Set runtime mode to Manual Control to be called from the main loop
    runtime_mode = manual_control;
}

void setStepperDelay()
{
    // Read two bytes for the time to delay pulses to the stepper
    // The time is measured in microseconds and should be in the range 500-1200ish
    return;
}
