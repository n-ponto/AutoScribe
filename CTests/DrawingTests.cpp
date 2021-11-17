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

extern const volatile bool continuedrawing();

// Globals from the Mocks
extern struct digitalWriteCalls DigitalWriteCalls;
extern std::queue<Point> *serialQueue;
extern bool continueDrawing;
int drawLoopCounter;

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

    assert(DigitalWriteCalls.top.stp.high == 1,
           "step top after first call");
    assert(DigitalWriteCalls.bot.stp.high == 1,
           "step bottom after first call");

    assert(DigitalWriteCalls.top.stp.low == 0,
           "no step low after first call");
    assert(DigitalWriteCalls.bot.stp.low == 0,
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

    assert(DigitalWriteCalls.top.stp.high == 1,
           "step top after second call");
    assert(DigitalWriteCalls.bot.stp.high == 1,
           "step bottom after second call");

    assert(DigitalWriteCalls.top.stp.low == 1,
           "no step low after second call");
    assert(DigitalWriteCalls.bot.stp.low == 1,
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
    assert(DigitalWriteCalls.enable.low == 1,
           "enable on start");
    assert(DigitalWriteCalls.enable.high == 1,
           "disable on end");
}

void moveOneThenStop()
{
    resetDigitalWriteCalls();
    Point pts[] = {{1, 0}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);

    drawLoopCounter = 1;
    drawing();

    // Check enable on start
    assert(DigitalWriteCalls.enable.low == 1,
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
    assert(DigitalWriteCalls.top.stp.high == 1);
    assert(DigitalWriteCalls.bot.stp.totalCallCount == 0);

    drawingInterrupt();

    // Check ended
    assert(DigitalWriteCalls.enable.high == 1,
           "disable on end");
}

void horizontal()
{
    const int16_t dist = 10;
    Point pts[] = {{dist, 0}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);

    resetDigitalWriteCalls();
    // Read in both points
    assert(q.size() == 2);
    drawLoopCounter = 2;
    drawing();
    assert(q.size() == 0);

    while (continueDrawing)
        drawingInterrupt();

    // Never had to change directions
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    // Never moved Y axis
    assert(DigitalWriteCalls.bot.stp.totalCallCount == 0);
    // Move X axis 10 times
    assert(DigitalWriteCalls.top.stp.high == dist);
    assert(DigitalWriteCalls.top.stp.low == dist);
}

void vertical()
{
    const int16_t dist = 17;
    Point pts[] = {{0, dist}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);

    // Read in both points
    assert(q.size() == 2);
    drawLoopCounter = 2;
    drawing();
    assert(q.size() == 0);

    while (continueDrawing)
        drawingInterrupt();

    // Never had to change directions
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    // Never moved X axis
    assert(DigitalWriteCalls.top.stp.totalCallCount == 0);
    // Move Y axis 10 times
    assert(DigitalWriteCalls.bot.stp.high == dist);
    assert(DigitalWriteCalls.bot.stp.low == dist);
}

void fortyFive()
{
    const int16_t dist = 7;
    Point pts[] = {{dist, dist}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);

    resetDigitalWriteCalls();
    drawLoopCounter = 2;
    drawing();

    while (continueDrawing)
        drawingInterrupt();

    // Never had to change directions
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    // Moved both axes
    assert(DigitalWriteCalls.top.stp.high == dist);
    assert(DigitalWriteCalls.top.stp.low == dist);
    assert(DigitalWriteCalls.bot.stp.high == dist);
    assert(DigitalWriteCalls.bot.stp.low == dist);
}

void slopeThird()
{
    const int16_t dist = 30;
    Point pts[] = {{dist, dist / 3}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);

    resetDigitalWriteCalls();
    drawLoopCounter = 2;
    drawing();

    while (continueDrawing)
    {
        drawingInterrupt();
        // Should always be more X axis steps than Y axis
        assert(DigitalWriteCalls.top.stp.high >= DigitalWriteCalls.bot.stp.high,
               "move X axis more than Y");
        // Never be more than 3x as many steps for X axis
        assert(DigitalWriteCalls.top.stp.high <= (DigitalWriteCalls.bot.stp.high + 1) * 3,
               "X axis at most 3x steps as Y");
    }

    // Never had to change directions
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    // Moved both axes
    assert(DigitalWriteCalls.top.stp.high == dist);
    assert(DigitalWriteCalls.top.stp.low == dist);
    assert(DigitalWriteCalls.bot.stp.high == dist / 3);
    assert(DigitalWriteCalls.bot.stp.low == dist / 3);
}

void slopeSteepThree()
{
    const int16_t dist = 30;
    Point pts[] = {{dist / 3, dist}, {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, 2);

    resetDigitalWriteCalls();
    drawLoopCounter = 2;
    drawing();

    while (continueDrawing)
    {
        drawingInterrupt();
        // Should always be more Y axis steps than X axis
        assert(DigitalWriteCalls.bot.stp.high >= DigitalWriteCalls.top.stp.high,
               "move Y axis more than X");
        // Never be more than 3x as many steps for Y axis
        assert(DigitalWriteCalls.bot.stp.high <= (DigitalWriteCalls.top.stp.high + 1) * 3,
               "Y axis at most 3x steps as X");
    }

    // Never had to change directions
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 1);
    assert(DigitalWriteCalls.top.dir.totalCallCount == 1);
    // Moved both axes
    assert(DigitalWriteCalls.bot.stp.high == dist);
    assert(DigitalWriteCalls.bot.stp.low == dist);
    assert(DigitalWriteCalls.top.stp.high == dist / 3);
    assert(DigitalWriteCalls.top.stp.low == dist / 3);
}

void weirdDiamond()
{
    const int numpts = 5;
    Point pts[] = {
        {21, 7},
        {10, 29},
        {7, 20},
        {21, 7},
        {STOP_DRAWING, 0}};

    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, numpts);

    resetDigitalWriteCalls();
    assert(q.size() == numpts);
    drawLoopCounter = 5;
    drawing();
    assert(q.size() == 0);

    while (continueDrawing)
        drawingInterrupt();

    // Change X axis direction twice
    assert(DigitalWriteCalls.top.dir.totalCallCount == 3);
    // Change Y axis direction once
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 2);

    // Move X axis
    assert(DigitalWriteCalls.top.stp.high == 21 + 11 + 3 + 14);
    assert(DigitalWriteCalls.top.stp.low == 21 + 11 + 3 + 14);
    // Move Y axis
    assert(DigitalWriteCalls.bot.stp.high == 7 + 22 + 9 + 13);
    assert(DigitalWriteCalls.bot.stp.low == 7 + 22 + 9 + 13);
}

int main(int argc, char *argv[])
{
    resetDigitalWriteCalls();

    struct test tests[] = {
        {writesLowAfterHigh, "writesLowAfterHigh"},
        {emergencyStop, "emergencyStop"},
        {moveOneThenStop, "moveOneThenStop"},
        {horizontal, "horizontal"},
        {vertical, "vertical"},
        {fortyFive, "fortyFive"},
        {slopeThird, "slopeThird"},
        {slopeSteepThree, "slopeSteepThree"},
        {weirdDiamond, "weirdDiamond"},
        {0, ""}};

    runTests("Drawing", tests);

    exit(0);
}
