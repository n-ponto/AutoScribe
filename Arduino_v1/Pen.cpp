#include <Servo.h>
#include <Arduino.h>
#include "Pen.h"

#define SERVO_PIN 11

#define DEFAULT_UP 50
#define DEFAULT_DOWN 70

Pen::Pen()
{
    Serial.println("Pen initialized.");
    this->servo = Servo();
    this->upAngle = DEFAULT_UP;
    this->downAngle = DEFAULT_DOWN;
    servo.attach(SERVO_PIN);
    servo.write(DEFAULT_UP);
}

void Pen::goTo(Angle angle)
{
    servo.write(angle);
}

void Pen::setUpAngle(Angle newUpAngle)
{
    this->upAngle = newUpAngle;
}

void Pen::setDownAngle(Angle newDownAngle)
{
    this->downAngle = newDownAngle;
}

void Pen::up()
{
    servo.write(upAngle);
}

void Pen::down()
{
    servo.write(downAngle);
}
