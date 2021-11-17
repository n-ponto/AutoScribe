#include <iostream>
#include <queue>

#include "DrawingMocks.h"
#include "../Arduino_v2/Drawing.h"
#include "../Arduino_v2/Stepper.h"

struct digitalWriteCalls DigitalWriteCalls;
_Serial Serial;
_Timer1 Timer1;

std::queue<Point> *serialQueue;

void setRuntimeMode(void (*fn)())
{
}

void acceptingCommands()
{
}

void delay(int)
{
}

void _Serial::print(const char *)
{
    return;
}

void _Serial::println(const char *)
{
    return;
}

void _Serial::print(unsigned long, int)
{
    return;
}

void _Serial::println(unsigned long, int)
{
    return;
}

int _Serial::available()
{
    return serialQueue->size() * POINTSZ;
}

size_t _Serial::readBytes(char *buffer, size_t length)
{
    if (length == POINTSZ)
    {
        Point pt = serialQueue->front();
        memcpy(buffer, &pt, length);
        serialQueue->pop();
        return length;
    }
    return 0xFFFFFFFF;
}

void _Timer1::attachInterrupt(void (*isr)())
{
}

void _Timer1::detachInterrupt()
{
}

void _Timer1::start()
{
}

void _Timer1::stop()
{
}

void resetDigitalWriteCalls()
{
    DigitalWriteCalls.top.dir = {0, 0, 0};
    DigitalWriteCalls.top.stp = {0, 0, 0};
    DigitalWriteCalls.bot.dir = {0, 0, 0};
    DigitalWriteCalls.bot.stp = {0, 0, 0};
    DigitalWriteCalls.enable = {0, 0, 0};
}

void delay(unsigned long);

void digitalWrite(uint8_t pin, uint8_t val)
{
    struct pin *p;
    switch (pin)
    {
    case (TOP_DIR_PIN):
        p = &DigitalWriteCalls.top.dir;
        break;
    case (BOT_DIR_PIN):
        p = &DigitalWriteCalls.bot.dir;
        break;
    case (TOP_STP_PIN):
        p = &DigitalWriteCalls.top.stp;
        break;
    case (BOT_STP_PIN):
        p = &DigitalWriteCalls.bot.stp;
        break;
    case (ENABLE_PIN):
        p = &DigitalWriteCalls.enable;
        break;
    default:
        std::cout << "[ERROR]: called with unexpected pin " << pin << std::endl;
        exit(1);
    }
    val ? p->high++ : p->low++;
    p->totalCallCount++;
}