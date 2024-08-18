#ifndef COMMANDS_H_
#define COMMANDS_H_

// 8 value bit to check if were switching runtime modes
#define RUNTIME_CHANGE 0b1000

// Command               Index   Encoding
void setPenRange();      // 1    0000 0001
void changePenAngle();   // 2    0000 0010
void setStepperDelay();  // 3    0000 0011
// Weird gap is so we can check the 8 bit to see if
// we should switch out of accepting commands
void enterDrawMode();           // 8    0000 1000
void enterManualControlMode();  // 9    0000 1001

#endif  // COMMANDS_H_