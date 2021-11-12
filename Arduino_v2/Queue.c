/*
Queue buffer implementation used by the Manual Control and Drawing modes
- circular FIFO design
- used to store instructions for drawing mode
- head indicates the next item out, tail indicates the next empty spot
*/
#include <string.h>
#include "Queue.h"

void 
queueInit(Queue *q, unsigned char *buffer, unsigned int memSz, unsigned int bufSz)
{
    q->buffer = buffer;
    q->bufSz = bufSz;
    q->memSz = memSz;

    q->head = 0;
    q->tail = 0;
    q->curSz = 0;
}

void
dequeue(Queue *q, void* out)
{
    if (q->curSz > 0)
    {
        unsigned char *addr = q->buffer + (q->head);
        memcpy(out, addr, q->memSz);
        q->head = (q->head + q->memSz) % q->bufSz;
        q->curSz--;
    } 
}

void
enqueue(Queue *q, void* in)
{
    unsigned char *addr = q->buffer + (q->tail);
    memcpy(addr, in, q->memSz);
    q->tail = (q->tail + q->memSz) % q->bufSz;
    q->curSz++;
}

char
isFull(Queue *q)
{
    // head should equal tail
    return q->curSz * q->memSz == q->bufSz;
}

char
isEmpty(Queue *q)
{
    // head should equal
    return q->curSz == 0;
}