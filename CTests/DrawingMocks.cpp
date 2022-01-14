#include "DrawingMocks.h"

#include <cstring>
#include <iostream>
#include <queue>

#include "../Arduino_v2/Drawing.h"
#include "../Arduino_v2/Hardware.h"
#include "TestingUtils.h"

// Object mocks
_Serial Serial;  // Mock of serial object
_Timer1 Timer2;  // Mock of timer object
Servo penServo;
uint8_t penUpAngle, penDownAngle;
uint16_t stepperDelay;

// Tracking state and events
struct digitalWriteCalls DigitalWriteCalls;  // Tracks calls to digital write
bool penStateUp;                             // Current state of the pen
bool xMovingCW, yMovingCW;                   // Motors are moving clockwise
Point currentPoint;                          // Current location of the end effector
bool queueLowFlag;                           // Flag indicates the drawing queue is low (should send more data)

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
void saveCanvas(const char *imageFileName) {
    generateBitmapImage((unsigned char *)image, IMAGE_HEIGHT, IMAGE_WIDTH, imageFileName);
}

// Sets all of the recorded digital write calls to zero
void resetDigitalWriteCalls() {
    DigitalWriteCalls.top.dir = {0, 0, 0};
    DigitalWriteCalls.top.stp = {0, 0, 0};
    DigitalWriteCalls.bot.dir = {0, 0, 0};
    DigitalWriteCalls.bot.stp = {0, 0, 0};
    DigitalWriteCalls.enable = {0, 0, 0};
}

// Resets the image drawing code to the start state
void resetCanvas() {
    const uint8_t color = 0;                               // Background color
    memset(image, color, IMAGE_HEIGHT * IMAGE_WIDTH * 3);  // Clear image
    currentPoint = {0, 0};                                 // Point to origin
    xMovingCW = yMovingCW = true;                          // Both motors moving clockwise
}

// Resets all of the mock values to their start state
void initMock() {
    resetCanvas();
    resetDigitalWriteCalls();
    penUpAngle = DEFAULT_UP;
    penDownAngle = DEFAULT_DOWN;
    stepperDelay = DEFAULT_STEPPER_DELAY;
    queueLowFlag = false;
}

void colorMove(int16_t x, int16_t y) {
    if (image[y][currentPoint.x][BLUE] == 255)
        return;  // Don't color over pixel
    image[y][currentPoint.x][BLUE] = 30;
    image[y][currentPoint.x][GREEN] = 30;
    image[y][currentPoint.x][RED] = 100;
}

void colorDraw(int16_t x, int16_t y) {
    image[y][currentPoint.x][BLUE] = 0;
    image[y][currentPoint.x][GREEN] = 0;
    image[y][currentPoint.x][RED] = 255;
}

void colorPixel() {
    int16_t x, y;
    x = currentPoint.x;
    y = IMAGE_HEIGHT - currentPoint.y;
    assert(0 <= currentPoint.x && currentPoint.x <= IMAGE_WIDTH, "x outside bounds");
    assert(0 <= y && y <= IMAGE_HEIGHT, "y outside bounds");

    penStateUp ? colorMove(x, y) : colorDraw(x, y);
}

/****************** MOCK FUNCTIONS (called by Drawing.cpp) ********************/

// Tracks the state of the pen on a servo write
void Servo::write(const uint8_t angle) {
    penStateUp = angle == penUpAngle;  // Lower angle is higher from paper
    if (!penStateUp) {
        colorPixel();
    }
}

// Returns the number of bytes avaliable to read from the serial receive buffer
int _Serial::available() {
    return serialQueue->size() * POINTSZ;
}

// Reads length bytes from serial into the buffer
size_t _Serial::readBytes(char *buffer, size_t length) {
    if (length == POINTSZ) {
        Point pt = serialQueue->front();
        memcpy(buffer, &pt, length);
        serialQueue->pop();
        return length;
    }
    return 0xFFFFFFFF;
}

size_t _Serial::write(uint8_t val) {
    // switch (val) {
    //     case (0xFF):
    //         queueLowFlag = true;
    //         break;
    //     default:
    //         printf("[ERROR] couldn't interpret value %X", val);
    // }
    return 1;
}

// Keeps track of the digital write calls and the
void digitalWrite(uint8_t pin, uint8_t val) {
    // Track digital write calls
    struct pin *p;
    switch (pin) {
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
            if (val)  // If write high (don't color again on write low)
            {
                currentPoint.x += xMovingCW ? 1 : -1;  // Change point location
                colorPixel();                          // Color the point
            }
            break;
        case (BOT_STP_PIN):
            p = &DigitalWriteCalls.bot.stp;
            if (val) {
                currentPoint.y += yMovingCW ? -1 : +1;
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

void _Timer1::attachInterrupt(void (*isr)()) {
}

void _Timer1::detachInterrupt() {
}

void _Timer1::start() {
}

void _Timer1::stop() {
}

void arduinoPrint(const char *str) {
    printf("[ARDUINO] %s\n", str);
}

void _Serial::print(const char *str) {
    arduinoPrint(str);
}

void _Serial::println(const char *str) {
    arduinoPrint(str);
}

void _Serial::print(unsigned long, int) {
    return;
}

void _Serial::println(unsigned long, int) {
    return;
}

void setRuntimeMode(void (*fn)()) {
}

void acceptingCommands() {
}

void delay(int) {
}

void displayDrawing() {
}

void updateInstructionCountDisplay(int) {

}
