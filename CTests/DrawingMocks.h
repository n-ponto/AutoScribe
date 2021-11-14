#ifndef DRAWING_MOCKS_H_
#define DRAWING_MOCKS_H_

#include <cstdint>
#include <cstddef>

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
struct action
{
    unsigned int totalCallCount, highCallCount, lowCallCount;
};
struct motor
{
    struct action dir, stp;
};
struct digitalWriteCalls
{
    struct motor top, bot;
};

void delay(unsigned long);
void digitalWrite(uint8_t pin, uint8_t val);

void drawingMockInit();

#endif //DRAWING_MOCKS_H_