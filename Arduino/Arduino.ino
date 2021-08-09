#include "Globals.h"
#include "Commands.h"
#include "Director.h"
#include "Receiver.h"
#include "Pen.h"

Director *director;
Receiver *receiver;
Pen *pen;

void setup()
{
    receiver = new Receiver();
    director = new Director();
    pen = new Pen();
}

void loop()
{
    Command command = (Command)receiver->readByte();
    HANDLE(command);
}

void setPenRange()
{
    Serial.println("setPenRange");
    Angle min, max;
    min = receiver->readByte();
    max = receiver->readByte();
    pen->setUpAngle(min);
    pen->setDownAngle(max);
}

void changePenAngle()
{
    Serial.println("changePenAngle");
    Angle angle = receiver->readByte();
    pen->goTo(angle);
}

void moveToCoordinate()
{
    Serial.println("Not done.");
}

void resetHome()
{
    Serial.println("resetHome");
    director->resetHome();
}

void draw()
{
    Serial.println("draw");
}

// Enters manual step mode:
// Takes a direction (as defined by Direction struct)
// If the byte is null then stop
void manualStep()
{
    Serial.println("Entering manual step mode");
    uint8_t b;
    director->enable();
    while(true)
    {
        b = receiver->readByte();
        switch(b)
        {
            case(END): 
                Serial.println("Ending manual step mode"); 
                director->disable();
                return;
            case(PEN_UP): pen->up(); break;
            case(PEN_DOWN): pen->down(); break;
            default:
                director->move(b, 1);
                break;
        }
    }
}