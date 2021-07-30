#include <Arduino.h>
#include "StepperController.h"

StepperController::StepperController()
{
    Serial.print("Stepper controller initialized.");
    pinMode(STEP_TOP, OUTPUT);
    pinMode(DIR_TOP,  OUTPUT);
    pinMode(STEP_BOT, OUTPUT);
    pinMode(DIR_BOT,  OUTPUT);
    pinMode(ENABLE,OUTPUT);
    digitalWrite(ENABLE, HIGH); // disable at the beginning
    wait = 700;
    enabled = false;
}

void StepperController::move(Direction d, uint16_t steps)
{
    if (d < 1 || d > 12)
    {
        // Can't be zero or have 3 bits selected
        return;
    }
    
    // If only one then both steppers will have to work
    bool b, t;
    switch(d)
    {
        case (UP):    t = b = CW;      break;
        case (DOWN):  t = b = CCW;     break;
        case (LEFT):  t = CCW; b = CW; break;
        case (RIGHT): t = CW; b = CCW; break;
        default:
            // Two directions selected
            bool spin = d & DOWN;
            bool stepper = (d & RIGHT) ^ spin;
            uint8_t stepPin = stepper ? STEP_TOP : STEP_BOT;
            uint8_t dirPin =  stepper ? DIR_TOP  : DIR_BOT;
            digitalWrite(dirPin, spin);
            for (uint16_t i = 0; i < steps; i++)
            {
                // Set both high
                digitalWrite(stepPin, HIGH);
                delayMicroseconds(wait);
                // Set both low
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

bool StepperController::areEnabled()
{
    return enabled;
}

void StepperController::enable()
{ 
    if (!enabled)
    {
        Serial.println("Steppers enabled");
        enabled = true;
        digitalWrite(ENABLE, LOW);
    } 
}

void StepperController::disable()
{
    if (enabled)
    {
        Serial.println("Steppers disabled");
        enabled = false;
        digitalWrite(ENABLE, HIGH);
    }
}

void StepperController::square(uint16_t steps)
{
    move(UP, steps);
    move(RIGHT, steps);
    move(DOWN, steps);
    move(LEFT, steps);
}

void StepperController::diamond(uint16_t steps)
{
    move(UP + RIGHT, steps);
    move(DOWN + RIGHT, steps);
    move(DOWN + LEFT, steps);
    move(UP + LEFT, steps);
}

void StepperController::home()
{
    disable();
    char c;
    while(Serial.available()) { Serial.readBytes(&c, 1); }
    Serial.print("Move the end to the bottom left and press enter when complete...");
    while(!Serial.available()) {}
    Serial.read();
    position.x = position.y = 0;

    printCoordinates();
}

void StepperController::printCoordinates()
{
    String output = "Current position: (" + String(position.x) + ", " + String(position.y) + ")\n";
    Serial.print(output);
}