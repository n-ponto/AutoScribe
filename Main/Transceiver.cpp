#include <Arduino.h>
#include <Servo.h>
#include "Transceiver.h"

void Transceiver::initialize(StepperController *stepper, Servo *servo)
{
    Serial.begin(9600);
    Serial.setTimeout(0.1);
    // Clear the read buffer on startup
    Serial.readString();
    this->stepper = stepper;
    this->servo = servo;
}

void Transceiver::handle()
{
    if (Serial.available() >= sizeof(Message))
    {
        Serial.readBytes((char *)(&message), sizeof(Message) );
        Serial.readString(); // Clear remaining buffer
        switch(message.command)
        {
            case MOVE_TO_COORDINATE:
                moveToCoordinate(message.param1, message.param2); break;
            case CHANGE_PEN_ANGLE:
                changePenAngle(message.param1); break;
            case RESET_HOME:
                resetHome(); break;
            default:
                break;
        }
    }
}

/*******************************************************************************
*******************************************************************************/
void Transceiver::moveToCoordinate(uint16_t x, uint16_t y)
{
    String notification = "Moving to (" + String(x) + ", " + String(y) + ")";
    Serial.println(notification);
    stepper->enable();
    stepper->travel(Point{x, y});
    stepper->disable();
}

void Transceiver::changePenAngle(uint16_t angle)
{
    String notification = "Changing pen angle to " + String(angle);
    Serial.println(notification);
    servo->write(angle);
}

void Transceiver::resetHome()
{
    stepper->resetHome();
}

