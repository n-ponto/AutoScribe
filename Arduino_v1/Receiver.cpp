#include <Arduino.h>
#include <Servo.h>
#include "Receiver.h"
#include "Globals.h"

Receiver::Receiver()
{
    Serial.begin(9600);
    Serial.setTimeout(0.1);
    Serial.println("Receiver initialized.");
    Serial.readString(); // Clear the read buffer on startup
}

uint8_t Receiver::readByte()
{
    while(!Serial.available()) {}
    uint8_t b = 0xF;
    Serial.readBytes((char *)&b, 1);
    return b;
}

Point Receiver::readPoint()
{
    while(Serial.available() < 4) {}
    Point p;
    Serial.readBytes((char *)&p, sizeof(Point));
    return p;
}

uint16_t Receiver::readPair()
{
    while(Serial.available() < 2) {}
    uint16_t p = 0xFF;
    Serial.readBytes((char*)&p, 2);
    return p;
}