#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstring>

#include "../Arduino_v2/Stepper.h"
#include "DrawingMocks.h"
#include "TestingUtils.h"

// Prototypes for Drawing.cpp functions for testing
void drawing();
void drawingInterrupt();

/****************** STRUCTS FROM DRAWING.CPP ******************/

typedef struct
{
    int16_t x, y;
} Point;

extern struct ds
{
    Point pt;
    int16_t i;
    int16_t absx;
    int16_t slopeError;
    int16_t c, m;
    bool swapxy;
    int8_t x_chnage, y_change;
} drawState;

extern struct ns
{
    uint8_t stepPin, write_value;
    bool changeXDir, changeYDir;
    uint8_t newXDir, newYDir;
} nextStep;

// Globals from the Mocks
extern struct digitalWriteCalls DigitalWriteCalls;

/****************** INTERRUPT TESTS ******************/

void writesLowAfterHigh()
{
    drawState = {0, 0, 0, 0, 0};
    nextStep.write_value = HIGH;
    nextStep.changeXDir = false;
    nextStep.changeYDir = false;
    nextStep.stepPin = 0xFF; // STEP_BOTH

    drawingInterrupt(); // Call interrupt

    // Should keep the direction the same, write high to both motors
    assert(DigitalWriteCalls.top.dir.totalCallCount == 0,
           "same top direction after call");
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 0,
           "save bottom direction after call");

    assert(DigitalWriteCalls.top.stp.highCallCount == 1,
           "step top after first call");
    assert(DigitalWriteCalls.bot.stp.highCallCount == 1,
           "step bottom after first call");

    assert(DigitalWriteCalls.top.stp.lowCallCount == 0,
           "no step low after first call");
    assert(DigitalWriteCalls.bot.stp.lowCallCount == 0,
           "no step bottom low after first call");

    // Should set the write value to LOW
    assert(nextStep.write_value == LOW,
           "set next call to low");

    drawingInterrupt(); // Call interrupt

    // Direction same, low to both motors
    assert(DigitalWriteCalls.top.dir.totalCallCount == 0,
           "same top direction after second call");
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 0,
           "save bottom direction after second call");

    assert(DigitalWriteCalls.top.stp.highCallCount == 1,
           "step top after second call");
    assert(DigitalWriteCalls.bot.stp.highCallCount == 1,
           "step bottom after second call");

    assert(DigitalWriteCalls.top.stp.lowCallCount == 1,
           "no step low after second call");
    assert(DigitalWriteCalls.bot.stp.lowCallCount == 1,
           "no step bottom low after second call");
    assert(false, "should fail");
}

/****************** DRAWING TESTS ******************/
void moveOne()
{
    return;
}

int main(int argc, char *argv[])
{
    drawingMockInit();

    struct test tests[] = {
        {writesLowAfterHigh, "writesLowAfterHigh"},
        {0, ""},
    };

    printf("Drawing Tests Starting\n");

    runTests(tests);

    exit(0);
}
