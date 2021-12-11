#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>

#include "../Arduino_v2/Hardware.h"
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
    initMock();

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

/******************************* DRAWING TESTS ******************************/

void queuePoints(Point *pts, const int count)
{
    initMock();
    for (int i = 0; i < count; i++)
    {
        serialQueue->push(pts[i]);
    }
}

void emergencyStop()
{
    initMock();
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
    initMock();
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

void triangle()
{
    const int numpts = 5;
    Point pts[] = {
        {21, 7},
        {10 | (MOVE_PEN), 29},  // pen down
        {7, 20},
        {21, 7},
        {STOP_DRAWING, 0}};

    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, numpts);

    assert(q.size() == numpts, "initial queue size matches numpts");
    drawLoopCounter = 5;
    drawing();
    assert(q.size() == 0, "queue emptied by drawing loop");

    while (continueDrawing)
        drawingInterrupt();

    assert(DigitalWriteCalls.top.dir.totalCallCount == 3, "Change X axis direction twice");
    assert(DigitalWriteCalls.bot.dir.totalCallCount == 2, "Change Y axis direction once");

    assert(DigitalWriteCalls.top.stp.high == 21 + 11 + 3 + 14, "X axis step high");
    assert(DigitalWriteCalls.top.stp.low == 21 + 11 + 3 + 14, "X axis step low");

    assert(DigitalWriteCalls.bot.stp.high == 7 + 22 + 9 + 13, "Y axis step high");
    assert(DigitalWriteCalls.bot.stp.low == 7 + 22 + 9 + 13, "Y axis step low");
}

/******************************* VISUALIZATION ******************************/

void vizN()
{
    const int numpts = 5;
    Point pts[] = {
        {100, 100},
        {100 | (MOVE_PEN), 200},
        {200, 100},
        {200, 200},
        {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, numpts);

    drawLoopCounter = numpts;
    drawing();
    assert(q.size() == 0);

    while (continueDrawing)
        drawingInterrupt();
}

void vizNoah()
{
    const int numpts = 21;
    Point pts[] = {
        // N
        {100, 100},
        {100 | (MOVE_PEN), 200}, // put pen down then move
        {200, 100},
        {200, 200},
        // O
        {(MOVE_PEN) | (PEN_UP) | 210, 100}, 
        {(MOVE_PEN) | 310, 100},
        {310, 200},
        {210, 200},
        {210, 100},
        // A
        {(MOVE_PEN) | (PEN_UP) | 320, 100}, 
        {(MOVE_PEN) | 370, 200},
        {429, 100}, // bottom right
        {(MOVE_PEN) | (PEN_UP) | 320, 140},
        {(MOVE_PEN) | 420, 140},
        // H
        {(MOVE_PEN) | (PEN_UP) | 430, 200},  // left line
        {(MOVE_PEN) | 430, 100},
        {(MOVE_PEN) | (PEN_UP) | 530, 200},  // right line
        {(MOVE_PEN) | 530, 100},
        {(MOVE_PEN) | (PEN_UP) | 430, 150},  // horizontal line
        {(MOVE_PEN) | 530, 150},
        {STOP_DRAWING, 0}};
    std::queue<Point> q;
    serialQueue = &q;
    queuePoints(pts, numpts);
    assert(q.size() == numpts, "initial queue size matches numpts");

    drawLoopCounter = numpts;
    drawing();
    assert(q.size() == 0);

    while (continueDrawing)
        drawingInterrupt();
}

void vizFile()
{
    // Get file path from user
    std::cout << "File path: ";
    std::string filePath; //  = "sample07.ncode";
    std::cin >> filePath;

    // Open file
    std::ifstream file;
    file.open(filePath);

    if (file.is_open())
        std::cout << "Opened file: " << filePath << std::endl;
    else
    {
        std::cout << "ERROR: couldn't open file" << std::endl;
        return;
    }

    std::string line;
    std::queue<Point> q;
    serialQueue = &q;
    bool movePen, penUp;
    movePen = false;
    penUp = false;
    unsigned long estimatedSteps = 0;
    uint16_t px, py;
    px = py = 0;
    while (std::getline(file, line))
    {
        if (line.compare("UP") == 0)
        {
            movePen = true;
            penUp = true;
        }
        else if (line.compare("DN") == 0)
        {
            movePen = true;
        }
        else
        {
            int pos = line.find(" ");
            assert(0 < pos && pos < 5, "space in range");
            int16_t x, y;
            x = stoi(line.substr(0, pos));
            y = stoi(line.substr(pos+1, line.length()));
            estimatedSteps += std::max(std::abs(x-px), std::abs(y-py));

            // Encode the x value
            x &= ~FLAG_MASK; // remove flag bits
            if (movePen)
            {
                x |= MOVE_PEN;
                if (penUp) {
                    x |= PEN_UP;
                }
                movePen = penUp = false;
            }

            // std::cout << "X: " << x << " Y: " << y << std::endl;
            Point newPt = {x, y};
            q.push(newPt);
        }
    }
    file.close();
    q.push({STOP_DRAWING, 0});

    std::cout << "Done generating queue of size " << q.size() << "\n";
    std::cout << "Estimated steps " << estimatedSteps << std::endl;

    drawLoopCounter = q.size();
    drawing();
    assert(q.size() == 0);

    std::cout << "Running drawing interrupt\n";
    while (continueDrawing)
        drawingInterrupt();
}

void runVizualization(char * arg)
{
    std::string argstr = std::string(arg);
    std::cout << "Searching for viz function \"" << argstr << "\"\n";
    struct test images[] = {
        {vizN, "vizN"},
        {vizNoah, "vizNoah"},
        {vizFile, "vizFile"},
        {0, ""}};
    
    // Check if string matches any of the viz functions
    for (struct test *t = images; t->f != 0; t++)
        if (t->s.compare(argstr) == 0)
        {
            std::cout << "Vizualizing " << t->s << std::endl;
            t->f(); // Run the vizualization
            std::string savePath = t->s.append(".bmp");
            saveCanvas(savePath.c_str());
            std::cout << "Done creating " << savePath << std::endl;
            return;
        }
    
    std::cout << "ERROR: couldn't find viz function for \"" << argstr << "\"\n";
    std::cout << "Valid functions: ";
    for (struct test *t = images; t->f != 0; t++)
        std::cout << t->s;
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    initMock();

    if (argc < 2) // No other arguments passed
    {
        struct test tests[] = {
            {writesLowAfterHigh, "writesLowAfterHigh"},
            {emergencyStop, "emergencyStop"},
            {moveOneThenStop, "moveOneThenStop"},
            {horizontal, "horizontal"},
            {vertical, "vertical"},
            {fortyFive, "fortyFive"},
            {slopeThird, "slopeThird"},
            {slopeSteepThree, "slopeSteepThree"},
            {triangle, "triangle"},
            {0, ""}};

        runTests("Drawing", tests);
    }
    else // Vizualize argument passed
    {
        std::string input = argv[1];

        runVizualization(argv[1]);
    }

    exit(0);
}
