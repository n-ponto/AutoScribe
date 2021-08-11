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

// Only takes 11bits to represent a coordinate dimension
// Leaves 5bits in each 2byte coordinate
void draw()
{
    Serial.println("Entering drawing mode");
    Point pt;
    director->resetHome();
    director->enable();
    while(true)
    {
        // End if both coordinates are zero
        if (~(pt.x | pt.y))
        {
            Serial.println("Ending drawing mode");
            director->disable();
            return;
        }

        uint8_t switchVal = pt.x>>11;
        Serial.print("switchVal: ");
        Serial.println(switchVal, BIN);
        switch(switchVal)
        {
            case(DRAW_PEN_UP): pen->up(); break;
            case(DRAW_PEN_DOWN): pen->down(); break;
        }
        pt.x &= COORD_MASK;
        pt.y &= COORD_MASK;
        Serial.print("Point (");
        Serial.print(pt.x);
        Serial.print(", ");
        Serial.print(pt.y);
        Serial.println(")");
        director->travel(pt);
    }
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
            case(MAN_END): 
                Serial.println("Ending manual step mode"); 
                director->disable();
                return;
            case(MAN_PEN_UP): pen->up(); break;
            case(MAN_PEN_DOWN): pen->down(); break;
            default:
                director->move(b, 1);
                break;
        }
    }
}

void setStepperDelay()
{
    uint16_t b = receiver->readPair();
    director->setDelay(b);
    Serial.print("Set stepper delay to ");
    Serial.println(b, DEC);
}