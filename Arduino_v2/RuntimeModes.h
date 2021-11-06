#ifndef _ACCEPTING_COMMANDS_H
#define _ACCEPTING_COMMANDS_H

void (*runtime_mode)(void);  // The current runtime mode

void accepting_commands();
void manual_control();
void drawing();

#endif //_ACCEPTING_COMMANDS_H