#ifndef RECEIVER_H_
#define RECEIVER_H_

#include "Globals.h"

class Receiver
{
public: 
    Receiver();
    uint8_t readByte();
    Point readPoint();
};

#endif // RECEIVER_H_