#ifndef DRAWING_H_
#define DRAWING_H_

#define POINTSZ 4   // Size of the point struct
#define BUFSZ 0x400 // Size in bytes of buffer to allocate for FIFO Queue

#define EMERGENCY_STOP 0x7FFF // Signal to stop drawing immediately
#define STOP_DRAWING 0x7FF0     // Signal to stop drawing when point dequeued

#define STEP_BOTH 0xFF // Encoding to step both the stepper motors

#define DIFF_SIGN(x, y) ((x ^ y) >> 15) // Check if one positive and one negative

// 4 byte struct transmitted from computer
typedef struct
{
    int16_t x, y;
} Point;

#endif //DRAWING_H_