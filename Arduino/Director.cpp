#include <Arduino.h>
#include "Globals.h"
#include "Director.h"

Director::Director()
{
    Serial.println("Director initialized.");
    pinMode(STEP_TOP, OUTPUT);
    pinMode(DIR_TOP, OUTPUT);
    pinMode(STEP_BOT, OUTPUT);
    pinMode(DIR_BOT, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    digitalWrite(ENABLE, HIGH); // disable at the beginning
    this->wait = 700;
    this->enabled = false;
    resetHome();
}

void Director::move(Direction d, uint16_t steps)
{
    if (d < 1 || d > 12)
    {
        // Can't be zero or have 3 bits selected
        return;
    }

    // If only one then both steppers will have to work
    bool b, t;
    switch (d)
    {
    case (UP):
        t = b = CW;
        break;
    case (DOWN):
        t = b = CCW;
        break;
    case (LEFT):
        t = CCW;
        b = CW;
        break;
    case (RIGHT):
        t = CW;
        b = CCW;
        break;
    default:
        // Two directions selected
        bool spin = d & DOWN;
        bool stepper = (d & RIGHT) ^ spin;
        uint8_t stepPin = stepper ? STEP_TOP : STEP_BOT;
        uint8_t dirPin = stepper ? DIR_TOP : DIR_BOT;
        digitalWrite(dirPin, spin);
        // If we're moving only one stepper we have to move it twice as far
        for (uint16_t i = 0; i < steps * 2; i++)
        {
            // Set one high
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(wait);
            // Set one low
            digitalWrite(stepPin, LOW);
            delayMicroseconds(wait);
        }
        return;
    }
    // Only one of the directions was selected

    // Set direction for both
    digitalWrite(DIR_TOP, t);
    digitalWrite(DIR_BOT, b);
    // Step both
    for (uint16_t i = 0; i < steps; i++)
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

bool Director::areEnabled()
{
    return enabled;
}

void Director::enable()
{
    if (!enabled)
    {
        Serial.println("Steppers enabled");
        enabled = true;
        digitalWrite(ENABLE, LOW);
    }
}

void Director::disable()
{
    if (enabled)
    {
        Serial.println("Steppers disabled");
        enabled = false;
        digitalWrite(ENABLE, HIGH);
    }
}

void Director::resetHome()
{
    disable();
    this->position.x = this->position.y = 0;
}

void Director::printCoordinates()
{
    String output = "Current position: (" + String(position.x) + ", " + String(position.y) + ")\n";
    Serial.print(output);
}

void Director::travel(const Point &pt)
{
    int16_t x, y;
    x = pt.x - position.x;
    y = pt.y - position.y;
    localTravel(x, y);
    position.x = pt.x;
    position.y = pt.y;
    printCoordinates();
}

void Director::localTravel(int16_t x, int16_t y)
{
    Serial.println("Travelling locally to: (" + String(x) + ", " + String(y) + ")");
    bool back = x < 0;
    bool down = y < 0;
    int16_t absx, absy;
    absx = back ? -1 * x : x;
    absy = down ? -1 * y : y;
    bool steep = absy > absx;

    // Set the output command
    Direction x_chng, y_chng;
    x_chng = back ? LEFT : RIGHT;
    y_chng = down ? DOWN : UP;
    Direction moveOne, moveBoth;
    moveBoth = x_chng + y_chng;
    moveOne = steep ? y_chng : x_chng;

    // Check if need to switch x and y
    if (steep)
    {
        int16_t save = absx;
        absx = absy;
        absy = save;
    }

    int16_t m, slope_error, c;
    m = 2 * absy;
    slope_error = m - absx;
    c = slope_error - absx;
    for (int16_t i = 0; i < absx; i++)
    {
        if (slope_error >= 0)
        {
            move(moveBoth, 1);
            slope_error += c;
        }
        else
        {
            move(moveOne, 1);
            slope_error += m;
        }
    }
}

/*******************************************************************************
 ******************************************************************************/

void Director::square(uint16_t steps)
{
    move(UP, steps);
    move(RIGHT, steps);
    move(DOWN, steps);
    move(LEFT, steps);
}

void Director::diamond(uint16_t steps)
{
    move(UP + RIGHT, steps);
    move(DOWN + RIGHT, steps);
    move(DOWN + LEFT, steps);
    move(UP + LEFT, steps);
}