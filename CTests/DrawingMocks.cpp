#include <iostream>

#include "DrawingMocks.h"
#include "../Arduino_v2/Stepper.h"

struct digitalWriteCalls DigitalWriteCalls;
_Serial Serial;
_Timer1 Timer1;

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
    return -1;
}

size_t _Serial::readBytes(char *buffer, size_t length)
{
    return 0;
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

void drawingMockInit()
{
    DigitalWriteCalls.top.dir = {0, 0, 0};
    DigitalWriteCalls.top.stp = {0, 0, 0};
    DigitalWriteCalls.bot.dir = {0, 0, 0};
    DigitalWriteCalls.bot.stp = {0, 0, 0};
}

void delay(unsigned long);

void digitalWrite(uint8_t pin, uint8_t val)
{
    struct action *m;
    switch (pin)
    {
    case (DIR_TOP):
        m = &DigitalWriteCalls.top.dir;
        break;
    case (DIR_BOT):
        m = &DigitalWriteCalls.bot.dir;
        break;
    case (STEP_TOP):
        m = &DigitalWriteCalls.top.stp;
        break;
    case (STEP_BOT):
        m = &DigitalWriteCalls.bot.stp;
        break;
    }

    if (val)
    {
        m->highCallCount++;
    }
    else
    {
        m->lowCallCount++;
    }
    m->totalCallCount++;
}