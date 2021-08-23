#ifndef INSTRUCTION_QUEUE_H_
#define INSTRUCTION_QUEUE_H_

#define QUEUE_SIZE 256

// Possible commands that can be stored in the instruction queue
#define PEN_UP_INSTRUCTION   0b00000001
#define PEN_DOWN_INSTRUCTION 0b00000010

#define TWO_BYTE  0x40
#define FOUR_BYTE 0x80

/***********
 * The instruction queue is populated by the main thread and read by the
 * interrupt so it knows how to move. 
 * 
 * The instruction queue holds 4 types of elements:
 * 1. A command (pen up, pen down, end)
 * 2. A 2 byte coordinate pair
 * 3. A 4 byte coordinate pair
 * 
 * The first 2 bits of each element is used as a siganture to differentiate
 * between the elements.
 */

typedef uint8_t InterruptCommand;

typedef struct instructionQueue
{
    uint8_t queue[QUEUE_SIZE];
    struct instructionQueue *next;
} InstructionQueue;

#endif // INSTRUCTION_QUEUE_H_