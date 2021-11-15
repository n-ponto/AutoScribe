#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstring>

#include "../Arduino_v2/Stepper.h"
#include "../Arduino_v2/Drawing.h"
#include "DrawingMocks.h"
#include "TestingUtils.h"

// Prototypes for Drawing.cpp functions for testing
void drawing();
void startDrawing();
void drawingLoop();
void endDrawing();
void drawingInterrupt();

/****************** STRUCTS FROM DRAWING.CPP ******************/

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
extern std::queue<Point> *serialQueue;

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
}

/****************** DRAWING TESTS ******************/

void queuePoints(Point *pts, const int count)
{
    resetDigitalWriteCalls();
    for (int i = 0; i < count; i++)
    {
        serialQueue->push(pts[i]);
    }
}
void emergencyStop()
{
    resetDigitalWriteCalls();
    Point es = {0x7FFF, 0};
    std::queue<Point> q;
    serialQueue = &q;
    q.push(es);
    drawing();
    assert(DigitalWriteCalls.enable.lowCallCount == 1,
           "enable on start");
    assert(DigitalWriteCalls.enable.highCallCount == 1,
           "disable on end");
}

void moveOneThenStop()
{
    resetDigitalWriteCalls();
    Point pts[] = {{1, 0}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);
    
    startDrawing();
    drawingLoop();
    // Check enable on start
    assert(DigitalWriteCalls.enable.lowCallCount == 1,
           "enable on start");
    drawingInterrupt();
    assert(drawState.swapxy == false, "line not steep");
    // Dont change direction or step either motor
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.top.stp.totalCallCount == 0);
    assert(DigitalWriteCalls.bot.stp.totalCallCount == 0);
    // Check the next command
    assert(nextStep.changeXDir == false);
    assert(nextStep.changeYDir == false);
    assert(nextStep.stepPin == TOP_STP_PIN);
    assert(nextStep.write_value == HIGH);

    drawingInterrupt();
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.top.stp.highCallCount == 1);
    assert(DigitalWriteCalls.bot.stp.totalCallCount == 0);

    drawingInterrupt();
    endDrawing();

    // Check ended
    assert(DigitalWriteCalls.enable.highCallCount == 1,
           "disable on end");
}

int main(int argc, char *argv[])
{
    resetDigitalWriteCalls();

    struct test tests[] = {
        {writesLowAfterHigh, "writesLowAfterHigh"},
        {emergencyStop, "emergencyStop"},
        {moveOneThenStop, "moveOneThenStop"},
        {0, ""}};

    printf("Drawing Tests Starting\n");

    runTests(tests);

    exit(0);
}
