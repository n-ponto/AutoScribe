#include <Arduino.h>
#include "StepperController.h"

unsigned long wait = 750;


/// Moves both motors in whatever direction is set
/// @param n number of times to step
void stepBoth(int n)
{
    for (int i = 0; i < n; i++) 
    {
        // Set both high
        digitalWrite(STEP_TOP, HIGH);
        digitalWrite(STEP_BOT, HIGH);
        delayMicroseconds(wait);
        // Set both low
        digitalWrite(STEP_TOP, LOW);
        digitalWrite(STEP_BOT, LOW); 
        delayMicroseconds(wait);
    }
}

void stepTop(int n)
{
    for (int i = 0; i < n; i++) 
    {
        digitalWrite(STEP_TOP, HIGH);
        delayMicroseconds(wait);
        digitalWrite(STEP_TOP, LOW);
        delayMicroseconds(wait);
    }
}

void stepBottom(int n)
{
    for (int i = 0; i < n; i++) 
    {
        digitalWrite(STEP_BOT, HIGH);
        delayMicroseconds(wait);
        digitalWrite(STEP_BOT, LOW);
        delayMicroseconds(wait);
    } 
}

/// Moves the end effector up n of times 
/// @param n number of times to step
void up(int n = 1)  
{
    // Both clockwise
    digitalWrite(DIR_TOP, CW);
    digitalWrite(DIR_BOT, CW);
    stepBoth(n);
}

/// Moves the end effector down n of times 
/// @param n number of times to step
void down(int n = 1)  
{
    // Both counterclockwise
    digitalWrite(DIR_TOP, CCW);
    digitalWrite(DIR_BOT, CCW);
    stepBoth(n);
}

void right(int n = 1)
{
    digitalWrite(DIR_TOP, CW);
    digitalWrite(DIR_BOT, CCW);
    stepBoth(n);
}

void left(int n = 1)
{
    digitalWrite(DIR_TOP, CCW);
    digitalWrite(DIR_BOT, CW);
    stepBoth(n);
}

void downRight(int n = 1)
{
    digitalWrite(DIR_BOT, CCW);
    stepBottom(n);
}

void upLeft(int n = 1)
{
    digitalWrite(DIR_BOT, CW);
    stepBottom(n);
}

void upRight(int n = 1)
{
    digitalWrite(DIR_TOP, CW);
    stepTop(n);
}

void downLeft(int n = 1)
{
    digitalWrite(DIR_TOP, CCW);
    stepTop(n);
}