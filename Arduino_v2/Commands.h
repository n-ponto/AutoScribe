#ifndef COMMANDS_H_
#define COMMANDS_H_

// Macro to call one of the functions in the array
#define HANDLE(x) functions[x]()
// 8 value bit to check if were switching runtime modes
#define RUNTIME_CHANGE 0b1000

// Command                  Index   Encoding
void setPenRange();         // 0    0000 0000
void changePenAngle();      // 1    0000 0001
void moveToCoordinate();    // 2    0000 0010
void resetHome();           // 3    0000 0011
void setStepperDelay();     // 4    0000 0100
// Weird gap is so we can check the 8 bit to see if  
// we should switch out of accepting commands
void enterDrawMode();       // 8    0000 1000
void enterManualControlMode(); // 9    0000 1001

void (*functions[])() = {
    setPenRange,            // 0
    changePenAngle,         // 1
    moveToCoordinate,       // 2
    resetHome,              // 3
    setStepperDelay,        // 4
    0, 0, 0,                // 5-7
    enterDrawMode,          // 8
    enterManualControlMode};   // 9

#endif // COMMANDS_H_