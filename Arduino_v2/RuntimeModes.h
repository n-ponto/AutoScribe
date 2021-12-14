#ifndef RUNTIME_MODES_H_
#define RUNTIME_MODES_H_

void acceptingCommands();
void manualControl();
void drawing();

void setRuntimeMode(void (*fn)(void));  // Defined in Arduino_v2.c

#endif //RUNTIME_MODES_H_