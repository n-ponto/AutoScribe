#ifndef RUNTIME_MODES_H_
#define RUNTIME_MODES_H_

void accepting_commands();
void manual_control();
void drawing();

void setRuntimeMode(void (*fn)(void));

#endif //RUNTIME_MODES_H_