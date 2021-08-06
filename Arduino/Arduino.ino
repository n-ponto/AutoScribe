#include "Globals.h"
#include "Director.h"
#include "Receiver.h"
#include "Pen.h"

Director *director;
Receiver *receiver;
Pen *pen;

typedef int (*fptr)();

// Local function parameters
void handle(Command);
void setPenRange();
void changePenAngle();
void resetHome();
void draw();
void manualStep();

void (*functions[])() = {
    setPenRange, changePenAngle, resetHome, draw, manualStep};

void setup()
{
    receiver = new Receiver();
    director = new Director();
    pen = new Pen();
}

void loop()
{
    Command command = (Command)receiver->readByte();
    functions[command]();
}

void setPenRange()
{
    Serial.println("setPenRange");
}
void changePenAngle()
{
    Serial.println("changePenAngle");
}
void resetHome()
{
    Serial.println("resetHome");
}
void draw()
{
    Serial.println("draw");
}
void manualStep()
{
    Serial.println("manualStep");
}