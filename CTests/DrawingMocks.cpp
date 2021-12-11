#include <iostream>
#include <queue>
#include <cstring>

#include "DrawingMocks.h"
#include "../Arduino_v2/Drawing.h"
#include "../Arduino_v2/Hardware.h"

// Object mocks
_Serial Serial; // Mock of serial object
_Timer1 Timer1; // Mock of timer object
Servo penServo;
uint8_t penUpAngle, penDownAngle;

// Tracking state and events
struct digitalWriteCalls DigitalWriteCalls; // Tracks calls to digital write
bool penStateUp;                            // Current state of the pen
bool xMovingCW, yMovingCW;                  // Motors are moving clockwise
Point currentPoint;                         // Current location of the end effector

// Mock of the Arduino's serial receive queue
std::queue<Point> *serialQueue;

#define BLUE 0
#define GREEN 1
#define RED 2
#define IMAGE_HEIGHT 1200
#define IMAGE_WIDTH 800
unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH][3];
void generateBitmapImage(unsigned char *image, int height, int width, const char *imageFileName);

/***************** TEST FUNCTIONS (called by DrawingTests.cpp) ****************/

// Saves the current state of the canvas
void saveCanvas(const char *imageFileName)
{
    generateBitmapImage((unsigned char *)image, IMAGE_HEIGHT, IMAGE_WIDTH, imageFileName);
}

// Sets all of the recorded digital write calls to zero
void resetDigitalWriteCalls()
{
    DigitalWriteCalls.top.dir = {0, 0, 0};
    DigitalWriteCalls.top.stp = {0, 0, 0};
    DigitalWriteCalls.bot.dir = {0, 0, 0};
    DigitalWriteCalls.bot.stp = {0, 0, 0};
    DigitalWriteCalls.enable = {0, 0, 0};
}

// Resets the image drawing code to the start state
void resetCanvas()
{
    memset(image, 0, IMAGE_HEIGHT * IMAGE_WIDTH * 3); // Clear image
    currentPoint = {0, 0};                            // Point to origin
    xMovingCW = yMovingCW = true;                     // Both motors moving clockwise
}

// Resets all of the mock values to their start state
void initMock()
{
    resetCanvas();
    resetDigitalWriteCalls();
    penUpAngle = DEFAULT_UP;
    penDownAngle = DEFAULT_DOWN;
}

/****************** MOCK FUNCTIONS (called by Drawing.cpp) ********************/

// Tracks the state of the pen on a servo write
void Servo::write(const uint8_t angle)
{
    penStateUp = angle < 60;  // Lower angle is higher from paper
    if (!penStateUp)
    {
        image[currentPoint.y][currentPoint.x][RED] = 255;
    }
}

// Returns the number of bytes avaliable to read from the serial receive buffer
int _Serial::available()
{
    return serialQueue->size() * POINTSZ;
}

// Reads length bytes from serial into the buffer
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

void colorPixel()
{
    if (penStateUp) // If the pen is up then draw a gray line
    {
        if (image[currentPoint.y][currentPoint.x][RED] == 255)
            return; // Don't color over pixel
        image[currentPoint.y][currentPoint.x][BLUE] = 100;
        image[currentPoint.y][currentPoint.x][GREEN] = 100;
        image[currentPoint.y][currentPoint.x][RED] = 100;
    }
    else // If the pen is down then draw a red line
        image[currentPoint.y][currentPoint.x][RED] = 255;
}

// Keeps track of the digital write calls and the
void digitalWrite(uint8_t pin, uint8_t val)
{
    // Track digital write calls
    struct pin *p;
    switch (pin)
    {
    case (TOP_DIR_PIN):
        p = &DigitalWriteCalls.top.dir;
        xMovingCW = val == CW;
        break;
    case (BOT_DIR_PIN):
        p = &DigitalWriteCalls.bot.dir;
        yMovingCW = val == CW;
        break;
    case (TOP_STP_PIN):
        p = &DigitalWriteCalls.top.stp;
        if (val) // If write high (don't color again on write low)
        {
            currentPoint.x += xMovingCW ? 1 : -1; // Change point location
            colorPixel();                         // Color the point
        }
        break;
    case (BOT_STP_PIN):
        p = &DigitalWriteCalls.bot.stp;
        if (val)
        {
            currentPoint.y += yMovingCW ? 1 : -1;
            colorPixel();
        }
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

/*************************** UNIMPLEMENTED FUNCTIONS **********************/

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

void setRuntimeMode(void (*fn)())
{
}

void acceptingCommands()
{
}

void delay(int)
{
}
