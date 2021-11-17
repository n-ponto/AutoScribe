#ifndef DRAWING_MOCKS_H_
#define DRAWING_MOCKS_H_

#include <iostream>
#include <queue>
#include <cstdint>
#include <cstddef>

#include "../Arduino_v2/Drawing.h"

#define LOW 0x0
#define HIGH 0x1

#define HEX 16

void setRuntimeMode(void(*fn)());
void acceptingCommands();
void delay(int);

class _Serial
{
public:
    void print(const char *);
    void println(const char *);
    void print(unsigned long, int);
    void println(unsigned long, int);
    int available();
    size_t readBytes(char *buffer, size_t length);
};

extern class _Serial Serial;

class _Timer1
{
public:
    void attachInterrupt(void (*isr)());
    void detachInterrupt();
    void start();
    void stop();
};

extern class _Timer1 Timer1;

// Struct to keep track of calls to digital write
struct pin
{
    unsigned int totalCallCount, high, low;
};
struct motor
{
    struct pin dir, stp;
};
struct digitalWriteCalls
{
    struct motor top, bot;
    struct pin enable;
};

void delay(unsigned long);
void digitalWrite(uint8_t pin, uint8_t val);

void resetDigitalWriteCalls();

#endif //DRAWING_MOCKS_H_